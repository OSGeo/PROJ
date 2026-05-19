/* <<< Release Notice for library >>> */

#include "proj.h"
#include "proj_internal.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#ifdef PROJ_GIT_HASH
#define PROJ_GIT_REV_STR " (" PROJ_GIT_HASH ")"
#else
#define PROJ_GIT_REV_STR ""
#endif

char const pj_release[] =
    "Rel. " STR(PROJ_VERSION_MAJOR) "." STR(PROJ_VERSION_MINOR) "." STR(
        PROJ_VERSION_PATCH) PROJ_GIT_REV_STR ", "
                                             "September 15th, 2026";

const char *pj_get_release() { return pj_release; }
