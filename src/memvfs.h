#ifndef MEMVFS_H
#define MEMVFS_H

#include <sqlite3.h>

#ifdef __cplusplus
extern "C" {
#endif

int pj_sqlite3_memvfs_init(sqlite3_vfs *vfs, const char *vfs_name,
                           const void *buffer, size_t bufferSize);
void pj_sqlite3_memvfs_deallocate_user_data(sqlite3_vfs *vfs);

#ifdef __cplusplus
}
#endif

#endif /* MEMVFS_H */
