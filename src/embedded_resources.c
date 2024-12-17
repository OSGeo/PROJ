#include <stdint.h>
#include <string.h>

#include "embedded_resources.h"

#if USE_SHARP_EMBED

#include "PROJ_DB_SQL_MD5.h"

const unsigned char *pj_get_embedded_proj_db(unsigned int *pnSize) {
    (void)PROJ_DB_SQL_MD5;
    static const unsigned char proj_db[] = {
#embed PROJ_DB
    };
    *pnSize = (unsigned int)sizeof(proj_db);
    return proj_db;
}

#else

#include "file_embed/proj_db.h"
const unsigned char *pj_get_embedded_proj_db(unsigned int *pnSize) {
    *pnSize = proj_db_size;
    return proj_db_data;
}

#endif

#include "file_embed/embedded_resources.c"
