#include <errno.h>

#include <proj.h>
#include "proj_internal.h"
#include "projects.h"

/* Generic 3D forward operation */

XYZ pj_fwd3d(LPZ lpz, PJ *P) {
    PJ_COORD coo = {{0,0,0,0}};
    coo.lpz = lpz;

    if (!P->skip_fwd_prepare)
        coo = pj_fwd_prepare (P, coo);
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ().xyz;

    /* Do the transformation, using the lowest dimensional transformer feasible */
    if (P->fwd3d)
        coo.xyz = P->fwd3d(coo.lpz, P);
    else if (P->fwd4d)
        coo = P->fwd4d (coo, P);
    else {
        proj_errno_set (P, EINVAL);
        return proj_coord_error ().xyz;
    }
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ().xyz;

    if (!P->skip_fwd_finalize)
        coo = pj_fwd_finalize (P, coo);
    return coo.xyz;
}
