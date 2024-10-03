#include "embedded_resources.h"

#if USE_SHARP_EMBED
const unsigned char *pj_get_embedded_proj_db(unsigned int *pnSize) {
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
