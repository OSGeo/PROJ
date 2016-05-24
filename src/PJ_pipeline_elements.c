/***********************************************************************

               Elemental datum shift operations

                 Thomas Knudsen, 2016-05-24

************************************************************************

**Elements**

STUB CODE

************************************************************************

Thomas Knudsen, thokn@sdfe.dk, 2016-05-24

***********************************************************************/

#define PJ_LIB__
#include <projects.h>
#include <geocent.h>
#include <PJ_pipeline.h>
#include <assert.h>
#include <stddef.h>
#include <errno.h>
PROJ_HEAD(add,     "Add a constant to the given coordinate (3 parameter shift)");











static COORDINATE add_3d (COORDINATE point, int direction, PJ *P) {
/*    long ret;
	double X, Y, Z, LAM, PHI;
	COORDINATE result = {{HUGE_VAL, HUGE_VAL, HUGE_VAL}};
	COORDINATE delta  = {{0, 0, 0}};
*/
    return point;
}




static XYZ add_forward_3d (LPZ lpz, PJ *P) {
    COORDINATE point;
    point.lpz = lpz;
    point = add_3d (point, 0, P);
    return point.xyz;
}

static LPZ add_reverse_3d (XYZ xyz, PJ *P) {
    COORDINATE point;
    point.xyz = xyz;
    point = add_3d (point, 1, P);
    return point.lpz;
}

static XY add_forward (LP lp, PJ *P) {
    COORDINATE point;
    point.lp = lp;
    point.lpz.z = 0;

    point = add_3d (point, 0, P);
    return point.xy;
}

static LP add_reverse (XY xy, PJ *P) {
    COORDINATE point;
    point.xy = xy;
    point.xyz.z = 0;

    point = add_3d (point, 1, P);
    return point.lp;
}




static void *add_freeup (PJ *P, int errlev) {         /* Destructor */
    if (0==P)
        return 0;

    pj_ctx_set_errno (P->ctx, errlev);

    if (0==P->opaque)
        return pj_dealloc (P);

    pj_dealloc (P->opaque);
    return pj_dealloc(P);
}


/* Adapts pipeline_freeup to the format defined for the PJ object */
static void freeup (PJ *P) {
    add_freeup (P, 0);
    return;
}




PJ *PROJECTION(add) {

    P->fwd3d = add_forward_3d;
    P->inv3d = add_reverse_3d;
    P->fwd   = add_forward;
    P->inv   = add_reverse;

    P->opaque = 0;

    return P;
}


/* selftest stub */
int pj_add_selftest (void) {return 0;}
