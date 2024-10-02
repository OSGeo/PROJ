#ifndef MEMVFS_H
#define MEMVFS_H

#include <sqlite3.h>

#ifdef __cplusplus
extern "C" {
#endif

int pj_sqlite3_memvfs_init(sqlite3_vfs *vfs, const char *vfs_name);

#ifdef __cplusplus
}
#endif

#endif /* MEMVFS_H */
