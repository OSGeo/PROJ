#include "embedded_resources.h"

const unsigned char *pj_get_embedded_proj_db(unsigned int *pnSize) {
    static const unsigned char proj_db[] = {
#embed PROJ_DB
    };
    *pnSize = (unsigned int)sizeof(proj_db);
    return proj_db;
}
