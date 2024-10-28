#include "embedded_resources.h"

#if USE_SHARP_EMBED
const unsigned char *pj_get_embedded_proj_db(unsigned int *pnSize) {
    static const unsigned char proj_db[] = {
#embed PROJ_DB
    };
    *pnSize = (unsigned int)sizeof(proj_db);
    return proj_db;
}

const char *pj_get_embedded_proj_ini(unsigned int *pnSize) {
    static const char proj_ini[] = {
#embed PROJ_INI
    };
    *pnSize = (unsigned int)sizeof(proj_ini);
    return proj_ini;
}

#else

#include "file_embed/proj_db.h"
const unsigned char *pj_get_embedded_proj_db(unsigned int *pnSize) {
    *pnSize = proj_db_size;
    return proj_db_data;
}

#include "file_embed/proj_ini.h"
const char *pj_get_embedded_proj_ini(unsigned int *pnSize) {
    *pnSize = proj_ini_size;
    return (const char *)proj_ini_data;
}

#endif
