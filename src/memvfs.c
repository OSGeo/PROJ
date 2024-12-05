/* Derived from https://www.sqlite.org/src/doc/tip/ext/misc/memvfs.c */
/*
** 2016-09-07
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
** This is an in-memory VFS implementation.  The application supplies
** a chunk of memory to hold the database file.
**
** Because there is place to store a rollback or wal journal, the database
** must use one of journal_mode=MEMORY or journal_mode=NONE.
**
** USAGE:
**
**    sqlite3_open_v2("file:/whatever?ptr=0xf05538&sz=14336&max=65536", &db,
**                    SQLITE_OPEN_READWRITE | SQLITE_OPEN_URI,
**                    "memvfs");
**
** These are the query parameters:
**
**    ptr=          The address of the memory buffer that holds the database.
**
**    sz=           The current size the database file
**
**    maxsz=        The maximum size of the database.  In other words, the
**                  amount of space allocated for the ptr= buffer.
**
** The ptr= and sz= query parameters are required.  If maxsz= is omitted,
** then it defaults to the sz= value.  Parameter values can be in either
** decimal or hexadecimal.  The filename in the URI is ignored.
*/
#include <sqlite3.h>

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "memvfs.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

#ifdef _MSC_VER
#pragma warning(push)
// Ignore unreferenced formal parameter
#pragma warning(disable : 4100)
#endif

/*
** Forward declaration of objects used by this utility
*/
typedef struct sqlite3_vfs MemVfs;
typedef struct MemFile MemFile;

typedef struct MemVfsAppData {
    const unsigned char *buffer;
    size_t bufferSize;
    sqlite3_vfs *pBaseVFS;
} MemVfsAppData;

/* Access to a lower-level VFS that (might) implement dynamic loading,
** access to randomness, etc.
*/
#define ORIGVFS(p) (((MemVfsAppData *)((p)->pAppData))->pBaseVFS)

/* An open file */
struct MemFile {
    sqlite3_file base;          /* IO methods */
    sqlite3_int64 sz;           /* Size of the file */
    sqlite3_int64 szMax;        /* Space allocated to aData */
    const unsigned char *aData; /* content of the file */
};

/*
** Methods for MemFile
*/
static int memClose(sqlite3_file *);
static int memRead(sqlite3_file *, void *, int iAmt, sqlite3_int64 iOfst);
static int memWrite(sqlite3_file *, const void *, int iAmt,
                    sqlite3_int64 iOfst);
static int memTruncate(sqlite3_file *, sqlite3_int64 size);
static int memSync(sqlite3_file *, int flags);
static int memFileSize(sqlite3_file *, sqlite3_int64 *pSize);
static int memLock(sqlite3_file *, int);
static int memUnlock(sqlite3_file *, int);
static int memCheckReservedLock(sqlite3_file *, int *pResOut);
static int memFileControl(sqlite3_file *, int op, void *pArg);
static int memSectorSize(sqlite3_file *);
static int memDeviceCharacteristics(sqlite3_file *);
static int memShmMap(sqlite3_file *, int iPg, int pgsz, int, void volatile **);
static int memShmLock(sqlite3_file *, int offset, int n, int flags);
static void memShmBarrier(sqlite3_file *);
static int memShmUnmap(sqlite3_file *, int deleteFlag);
static int memFetch(sqlite3_file *, sqlite3_int64 iOfst, int iAmt, void **pp);
static int memUnfetch(sqlite3_file *, sqlite3_int64 iOfst, void *p);

/*
** Methods for MemVfs
*/
static int memOpen(sqlite3_vfs *, const char *, sqlite3_file *, int, int *);
static int memDelete(sqlite3_vfs *, const char *zName, int syncDir);
static int memAccess(sqlite3_vfs *, const char *zName, int flags, int *);
static int memFullPathname(sqlite3_vfs *, const char *zName, int, char *zOut);
static void *memDlOpen(sqlite3_vfs *, const char *zFilename);
static void memDlError(sqlite3_vfs *, int nByte, char *zErrMsg);
static void (*memDlSym(sqlite3_vfs *pVfs, void *p, const char *zSym))(void);
static void memDlClose(sqlite3_vfs *, void *);
static int memRandomness(sqlite3_vfs *, int nByte, char *zOut);
static int memSleep(sqlite3_vfs *, int microseconds);
static int memCurrentTime(sqlite3_vfs *, double *);
static int memGetLastError(sqlite3_vfs *, int, char *);
static int memCurrentTimeInt64(sqlite3_vfs *, sqlite3_int64 *);

static sqlite3_vfs mem_vfs = {
    2,                  /* iVersion */
    0,                  /* szOsFile (set when registered) */
    1024,               /* mxPathname */
    0,                  /* pNext */
    "memvfs",           /* zName */
    0,                  /* pAppData (set when registered) */
    memOpen,            /* xOpen */
    memDelete,          /* xDelete */
    memAccess,          /* xAccess */
    memFullPathname,    /* xFullPathname */
    memDlOpen,          /* xDlOpen */
    memDlError,         /* xDlError */
    memDlSym,           /* xDlSym */
    memDlClose,         /* xDlClose */
    memRandomness,      /* xRandomness */
    memSleep,           /* xSleep */
    memCurrentTime,     /* xCurrentTime */
    memGetLastError,    /* xGetLastError */
    memCurrentTimeInt64 /* xCurrentTimeInt64 */
};

static const sqlite3_io_methods mem_io_methods = {
    3,                        /* iVersion */
    memClose,                 /* xClose */
    memRead,                  /* xRead */
    memWrite,                 /* xWrite */
    memTruncate,              /* xTruncate */
    memSync,                  /* xSync */
    memFileSize,              /* xFileSize */
    memLock,                  /* xLock */
    memUnlock,                /* xUnlock */
    memCheckReservedLock,     /* xCheckReservedLock */
    memFileControl,           /* xFileControl */
    memSectorSize,            /* xSectorSize */
    memDeviceCharacteristics, /* xDeviceCharacteristics */
    memShmMap,                /* xShmMap */
    memShmLock,               /* xShmLock */
    memShmBarrier,            /* xShmBarrier */
    memShmUnmap,              /* xShmUnmap */
    memFetch,                 /* xFetch */
    memUnfetch                /* xUnfetch */
};

/*
** Close an mem-file.
**
** The pData pointer is owned by the application, so there is nothing
** to free.
*/
static int memClose(sqlite3_file *pFile) { return SQLITE_OK; }

/*
** Read data from an mem-file.
*/
static int memRead(sqlite3_file *pFile, void *zBuf, int iAmt,
                   sqlite_int64 iOfst) {
    MemFile *p = (MemFile *)pFile;
    memcpy(zBuf, p->aData + iOfst, iAmt);
    return SQLITE_OK;
}

/*
** Write data to an mem-file.
*/
static int memWrite(sqlite3_file *pFile, const void *z, int iAmt,
                    sqlite_int64 iOfst) {
    return SQLITE_READONLY;
}

/*
** Truncate an mem-file.
*/
static int memTruncate(sqlite3_file *pFile, sqlite_int64 size) {
    return SQLITE_READONLY;
}

/*
** Sync an mem-file.
*/
static int memSync(sqlite3_file *pFile, int flags) { return SQLITE_OK; }

/*
** Return the current file-size of an mem-file.
*/
static int memFileSize(sqlite3_file *pFile, sqlite_int64 *pSize) {
    MemFile *p = (MemFile *)pFile;
    *pSize = p->sz;
    return SQLITE_OK;
}

/*
** Lock an mem-file.
*/
static int memLock(sqlite3_file *pFile, int eLock) { return SQLITE_OK; }

/*
** Unlock an mem-file.
*/
static int memUnlock(sqlite3_file *pFile, int eLock) { return SQLITE_OK; }

/*
** Check if another file-handle holds a RESERVED lock on an mem-file.
*/
static int memCheckReservedLock(sqlite3_file *pFile, int *pResOut) {
    *pResOut = 0;
    return SQLITE_OK;
}

/*
** File control method. For custom operations on an mem-file.
*/
static int memFileControl(sqlite3_file *pFile, int op, void *pArg) {
    MemFile *p = (MemFile *)pFile;
    int rc = SQLITE_NOTFOUND;
    if (op == SQLITE_FCNTL_VFSNAME) {
        *(char **)pArg = sqlite3_mprintf("mem(%p,%lld)", p->aData, p->sz);
        rc = SQLITE_OK;
    }
    return rc;
}

/*
** Return the sector-size in bytes for an mem-file.
*/
static int memSectorSize(sqlite3_file *pFile) { return 1024; }

/*
** Return the device characteristic flags supported by an mem-file.
*/
static int memDeviceCharacteristics(sqlite3_file *pFile) {
    return SQLITE_IOCAP_ATOMIC | SQLITE_IOCAP_POWERSAFE_OVERWRITE |
           SQLITE_IOCAP_SAFE_APPEND | SQLITE_IOCAP_SEQUENTIAL;
}

/* Create a shared memory file mapping */
static int memShmMap(sqlite3_file *pFile, int iPg, int pgsz, int bExtend,
                     void volatile **pp) {
    return SQLITE_IOERR_SHMMAP;
}

/* Perform locking on a shared-memory segment */
static int memShmLock(sqlite3_file *pFile, int offset, int n, int flags) {
    return SQLITE_IOERR_SHMLOCK;
}

/* Memory barrier operation on shared memory */
static void memShmBarrier(sqlite3_file *pFile) { return; }

/* Unmap a shared memory segment */
static int memShmUnmap(sqlite3_file *pFile, int deleteFlag) {
    return SQLITE_OK;
}

/* Fetch a page of a memory-mapped file */
static int memFetch(sqlite3_file *pFile, sqlite3_int64 iOfst, int iAmt,
                    void **pp) {
    MemFile *p = (MemFile *)pFile;
    *pp = (void *)(p->aData + iOfst);
    return SQLITE_OK;
}

/* Release a memory-mapped page */
static int memUnfetch(sqlite3_file *pFile, sqlite3_int64 iOfst, void *pPage) {
    return SQLITE_OK;
}

/*
** Open an mem file handle.
*/
static int memOpen(sqlite3_vfs *pVfs, const char *zName, sqlite3_file *pFile,
                   int flags, int *pOutFlags) {
    MemFile *p = (MemFile *)pFile;
    MemVfsAppData *appData = (MemVfsAppData *)(pVfs->pAppData);
    memset(p, 0, sizeof(*p));
    if ((flags & SQLITE_OPEN_MAIN_DB) == 0) {
        /* Modification w.r.t upstream: instead of returning SQLITE_CANTOPEN,
         * delegate to origin VFS. Typically for temporary file creation.
         */
        return ORIGVFS(pVfs)->xOpen(ORIGVFS(pVfs), zName, pFile, flags,
                                    pOutFlags);
    }
    if ((uintptr_t)(appData->buffer) !=
        (uintptr_t)sqlite3_uri_int64(zName, "ptr", 0)) {
        return SQLITE_CANTOPEN;
    }
    p->aData = appData->buffer;
    p->sz = sqlite3_uri_int64(zName, "sz", 0);
    if (p->sz < 0 || (size_t)p->sz != appData->bufferSize)
        return SQLITE_CANTOPEN;
    p->szMax = sqlite3_uri_int64(zName, "max", p->sz);
    if (p->szMax < p->sz)
        return SQLITE_CANTOPEN;
    pFile->pMethods = &mem_io_methods;
    return SQLITE_OK;
}

/*
** Delete the file located at zPath. If the dirSync argument is true,
** ensure the file-system modifications are synced to disk before
** returning.
*/
static int memDelete(sqlite3_vfs *pVfs, const char *zPath, int dirSync) {
    return SQLITE_IOERR_DELETE;
}

/*
** Test for access permissions. Return true if the requested permission
** is available, or false otherwise.
*/
static int memAccess(sqlite3_vfs *pVfs, const char *zPath, int flags,
                     int *pResOut) {
    *pResOut = 0;
    return SQLITE_OK;
}

/*
** Populate buffer zOut with the full canonical pathname corresponding
** to the pathname in zPath. zOut is guaranteed to point to a buffer
** of at least (INST_MAX_PATHNAME+1) bytes.
*/
static int memFullPathname(sqlite3_vfs *pVfs, const char *zPath, int nOut,
                           char *zOut) {
    sqlite3_snprintf(nOut, zOut, "%s", zPath);
    return SQLITE_OK;
}

/*
** Open the dynamic library located at zPath and return a handle.
*/
static void *memDlOpen(sqlite3_vfs *pVfs, const char *zPath) {
    return ORIGVFS(pVfs)->xDlOpen(ORIGVFS(pVfs), zPath);
}

/*
** Populate the buffer zErrMsg (size nByte bytes) with a human readable
** utf-8 string describing the most recent error encountered associated
** with dynamic libraries.
*/
static void memDlError(sqlite3_vfs *pVfs, int nByte, char *zErrMsg) {
    ORIGVFS(pVfs)->xDlError(ORIGVFS(pVfs), nByte, zErrMsg);
}

/*
** Return a pointer to the symbol zSymbol in the dynamic library pHandle.
*/
static void (*memDlSym(sqlite3_vfs *pVfs, void *p, const char *zSym))(void) {
    return ORIGVFS(pVfs)->xDlSym(ORIGVFS(pVfs), p, zSym);
}

/*
** Close the dynamic library handle pHandle.
*/
static void memDlClose(sqlite3_vfs *pVfs, void *pHandle) {
    ORIGVFS(pVfs)->xDlClose(ORIGVFS(pVfs), pHandle);
}

/*
** Populate the buffer pointed to by zBufOut with nByte bytes of
** random data.
*/
static int memRandomness(sqlite3_vfs *pVfs, int nByte, char *zBufOut) {
    return ORIGVFS(pVfs)->xRandomness(ORIGVFS(pVfs), nByte, zBufOut);
}

/*
** Sleep for nMicro microseconds. Return the number of microseconds
** actually slept.
*/
static int memSleep(sqlite3_vfs *pVfs, int nMicro) {
    return ORIGVFS(pVfs)->xSleep(ORIGVFS(pVfs), nMicro);
}

/*
** Return the current time as a Julian Day number in *pTimeOut.
*/
static int memCurrentTime(sqlite3_vfs *pVfs, double *pTimeOut) {
    return ORIGVFS(pVfs)->xCurrentTime(ORIGVFS(pVfs), pTimeOut);
}

static int memGetLastError(sqlite3_vfs *pVfs, int a, char *b) {
    return ORIGVFS(pVfs)->xGetLastError(ORIGVFS(pVfs), a, b);
}
static int memCurrentTimeInt64(sqlite3_vfs *pVfs, sqlite3_int64 *p) {
    return ORIGVFS(pVfs)->xCurrentTimeInt64(ORIGVFS(pVfs), p);
}

/*
 * Register the new VFS.
 */
int pj_sqlite3_memvfs_init(sqlite3_vfs *vfs, const char *vfs_name,
                           const void *buffer, size_t bufferSize) {
    memcpy(vfs, &mem_vfs, sizeof(mem_vfs));
    vfs->zName = vfs_name;
    sqlite3_vfs *defaultVFS = sqlite3_vfs_find(NULL);
    if (!defaultVFS)
        return SQLITE_ERROR;
    MemVfsAppData *appData =
        (MemVfsAppData *)sqlite3_malloc(sizeof(MemVfsAppData));
    appData->buffer = buffer;
    appData->bufferSize = bufferSize;
    appData->pBaseVFS = defaultVFS;
    vfs->pAppData = appData;
    vfs->szOsFile = sizeof(MemFile);
    /* Modification w.r.t upstream: as we might delegate file opening
     * to default VFS for temporary files, we need to make sure szOsFile is
     * the maximum of our own need and of the default VFS.
     */
    if (vfs->szOsFile < defaultVFS->szOsFile)
        vfs->szOsFile = defaultVFS->szOsFile;
    return sqlite3_vfs_register(vfs, 0);
}

void pj_sqlite3_memvfs_deallocate_user_data(sqlite3_vfs *vfs) {
    sqlite3_free(vfs->pAppData);
    vfs->pAppData = NULL;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
