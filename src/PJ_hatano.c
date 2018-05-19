#define PJ_LIB__

#include <math.h>

#include "proj.h"
#include "projects.h"

PROJ_HEAD(hatano, "Hatano Asymmetrical Equal Area") "\n\tPCyl, Sph.";

#define NITER   20
#define EPS 1e-7
#define ONETOL 1.000001
#define CN  2.67595
#define CS  2.43763
#define RCN 0.37369906014686373063
#define RCS 0.41023453108141924738
#define FYCN    1.75859
#define FYCS    1.93052
#define RYCN    0.56863737426006061674
#define RYCS    0.51799515156538134803
#define FXC 0.85
#define RXC 1.17647058823529411764


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    double th1, c;
    int i;
    (void) P;

    c = sin(lp.phi) * (lp.phi < 0. ? CS : CN);
    for (i = NITER; i; --i) {
        lp.phi -= th1 = (lp.phi + sin(lp.phi) - c) / (1. + cos(lp.phi));
        if (fabs(th1) < EPS) break;
    }
    xy.x = FXC * lp.lam * cos(lp.phi *= .5);
    xy.y = sin(lp.phi) * (lp.phi < 0. ? FYCS : FYCN);

    return xy;
}


static LP s_inverse (XY xy, PJ *P) {           /* Spheroidal, inverse */
    LP lp = {0.0,0.0};
    double th;

    th = xy.y * ( xy.y < 0. ? RYCS : RYCN);
    if (fabs(th) > 1.) {
        if (fabs(th) > ONETOL) {
            proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
            return lp;
        } else {
            th = th > 0. ? M_HALFPI : - M_HALFPI;
        }
    } else {
        th = asin(th);
    }

    lp.lam = RXC * xy.x / cos(th);
    th += th;
    lp.phi = (th + sin(th)) * (xy.y < 0. ? RCS : RCN);
    if (fabs(lp.phi) > 1.) {
        if (fabs(lp.phi) > ONETOL) {
            proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
            return lp;
        } else {
            lp.phi = lp.phi > 0. ? M_HALFPI : - M_HALFPI;
        }
    } else {
        lp.phi = asin(lp.phi);
    }

    return (lp);
}


PJ *PROJECTION(hatano) {
    P->es = 0.;
    P->inv = s_inverse;
    P->fwd = s_forward;

    return P;
}
