#define PJ_LIB__
#include <proj.h>
#include "proj_internal.h"
#include "projects.h"

/* Generic 2D forward operation */

XY pj_fwd(LP lp, PJ *P) {
    PJ_COORD coo = {{0,0,0,0}};
    coo.lp = lp;

    coo = pj_fwd_prepare (P, coo);
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ().xy;

    /* Do the transformation, using the lowest dimensional transformer available */
    if (P->fwd)
        coo.xy = P->fwd(coo.lp, P);
    else if (P->fwd3d)
        coo.xyz = P->fwd3d (coo.lpz, P);
    else if (P->fwd4d)
        coo = P->fwd4d (coo, P);
    else {
        proj_errno_set (P, EINVAL);
        return proj_coord_error ().xy;
    }

    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ().xy;
    return pj_fwd_finalize (P, coo).xy;
}
