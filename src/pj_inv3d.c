#define PJ_LIB__
#include <proj.h>
#include "proj_internal.h"
#include "projects.h"
#include <errno.h>

/* Generic 3D inverse operation */

LPZ pj_inv3d (XYZ xyz, PJ *P) {
    PJ_COORD coo = {{0,0,0,0}};
    coo.xyz = xyz;

    coo = pj_inv_prepare (P, coo);
    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ().lpz;

    /* Do the transformation, using the lowest dimensional transformer feasible */
    if (P->inv3d)
        coo.lpz = P->inv3d (coo.xyz, P);
    else if (P->inv4d)
        coo = P->inv4d (coo, P);
    else {
        proj_errno_set (P, EINVAL);
        return proj_coord_error ().lpz;
    }

    if (HUGE_VAL==coo.v[0])
        return proj_coord_error ().lpz;
    return pj_inv_finalize (P, coo).lpz;
}
