#include <errno.h>

#include <proj.h>
#include "proj_internal.h"
#include "projects.h"

/* Generic 2D inverse operation */

LP pj_inv(XY xy, PJ *P) {
    PJ_COORD coo = {{0,0,0,0}};
    coo.xy = xy;

    if (!P->skip_inv_prepare)
        coo = pj_inv_prepare (P, coo);
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ().lp;

    /* Do the transformation, using the lowest dimensional transformer available */
    if (P->inv)
        coo.lp = P->inv(coo.xy, P);
    else if (P->inv3d)
        coo.lpz = P->inv3d (coo.xyz, P);
    else if (P->inv4d)
        coo = P->inv4d (coo, P);
    else {
        proj_errno_set (P, EINVAL);
        return proj_coord_error ().lp;
    }
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ().lp;

    if (!P->skip_inv_finalize)
        coo = pj_inv_finalize (P, coo);
    return coo.lp;
}
