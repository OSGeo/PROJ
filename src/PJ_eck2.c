#define PJ_LIB__

#include <math.h>

#include "proj.h"
#include "projects.h"

PROJ_HEAD(eck2, "Eckert II") "\n\tPCyl. Sph.";

#define FXC     0.46065886596178063902
#define FYC     1.44720250911653531871
#define C13     0.33333333333333333333
#define ONEEPS  1.0000001


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    (void) P;

    xy.x = FXC * lp.lam * (xy.y = sqrt(4. - 3. * sin(fabs(lp.phi))));
    xy.y = FYC * (2. - xy.y);
    if ( lp.phi < 0.) xy.y = -xy.y;

    return (xy);
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    (void) P;

    lp.lam = xy.x / (FXC * ( lp.phi = 2. - fabs(xy.y) / FYC) );
    lp.phi = (4. - lp.phi * lp.phi) * C13;
    if (fabs(lp.phi) >= 1.) {
        if (fabs(lp.phi) > ONEEPS) {
            proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
            return lp;
        } else {
            lp.phi = lp.phi < 0. ? -M_HALFPI : M_HALFPI;
        }
    } else
        lp.phi = asin(lp.phi);
    if (xy.y < 0)
        lp.phi = -lp.phi;
    return (lp);
}



PJ *PROJECTION(eck2) {
    P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}
