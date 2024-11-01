#ifndef EMBEDDED_RESOURCES_H
#define EMBEDDED_RESOURCES_H

#ifdef __cplusplus
extern "C" {
#endif

const unsigned char *pj_get_embedded_proj_db(unsigned int *pnSize);
const char *pj_get_embedded_proj_ini(unsigned int *pnSize);

#ifdef __cplusplus
}
#endif

#endif /* EMBEDDED_RESOURCES_H */
