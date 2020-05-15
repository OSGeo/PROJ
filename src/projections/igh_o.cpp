#define PJ_LIB__

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"

PROJ_HEAD(igh_o, "Interrupted Goode Homolosine Oceanic View") "\n\tPCyl, Sph";

/*
This projection is a variant of the Interrupted Goode Homolosine
projection that emphasizes ocean areas. The projection is a
compilation of 12 separate sub-projections. Sinusoidal projections
are found near the equator and Mollweide projections are found at
higher latitudes. The transition between the two occurs at 40 degrees
latitude and is represented by `phi_boundary`.

Each sub-projection is assigned an integer label
numbered 1 through 12. Most of this code contains logic to assign
the labels based on latitude (phi) and longitude (lam) regions.
*/

C_NAMESPACE PJ *pj_sinu(PJ *), *pj_moll(PJ *);

/* 
Transition from sinusoidal to Mollweide projection
Latitude (phi): 40deg 44' 11.8" 
*/

static const double phi_boundary = (40 + 44/60. + 11.8/3600.) * DEG_TO_RAD;

static const double d10  =  10 * DEG_TO_RAD;
static const double d40  =  40 * DEG_TO_RAD;
static const double d50  =  50 * DEG_TO_RAD;
static const double d60  =  60 * DEG_TO_RAD;
static const double d70  =  70 * DEG_TO_RAD;
static const double d75  =  75 * DEG_TO_RAD;
static const double d90  =  90 * DEG_TO_RAD;
static const double d110 = 110 * DEG_TO_RAD;
static const double d120 = 120 * DEG_TO_RAD;
static const double d130 = 130 * DEG_TO_RAD;
static const double d150 = 150 * DEG_TO_RAD;
static const double d180 = 180 * DEG_TO_RAD;

static const double EPSLN = 1.e-10; /* allow a little 'slack' on zone edge positions */

namespace { // anonymous namespace
struct pj_opaque {
    struct PJconsts* pj[12]; \
    double dy0;
};
} // anonymous namespace


/*
Assign an integer index representing each of the 12 
sub-projection zones based on latitude (phi) and
longitude (lam) ranges.
*/

static PJ_XY igh_o_s_forward (PJ_LP lp, PJ *P) {           /* Spheroidal, forward */
    PJ_XY xy;
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(P->opaque);
    int z;

    if (lp.phi >=  phi_boundary) {
           if (lp.lam <=  -d110) z =  1;
      else if (lp.lam >=    d50) z =  3;
      else z = 2;
    }
    else if (lp.phi >=  0) {
           if (lp.lam <=  -d110) z =  4;
      else if (lp.lam >=    d50) z =  6;
      else z = 5;
    }
    else if (lp.phi >= -phi_boundary) {
           if (lp.lam <=  -d75) z =  7;
      else if (lp.lam >=   d90) z =  9;
      else z = 8;
    }
    else {
           if (lp.lam <=  -d75) z =  10;
      else if (lp.lam >=   d90) z =  12;
      else z = 11;
    }

    lp.lam -= Q->pj[z-1]->lam0;
    xy = Q->pj[z-1]->fwd(lp, Q->pj[z-1]);
    xy.x += Q->pj[z-1]->x0;
    xy.y += Q->pj[z-1]->y0;

    return xy;
}


static PJ_LP igh_o_s_inverse (PJ_XY xy, PJ *P) {           /* Spheroidal, inverse */
    PJ_LP lp = {0.0,0.0};
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(P->opaque);
    const double y90 = Q->dy0 + sqrt(2.0); /* lt=90 corresponds to y=y0+sqrt(2) */

    int z = 0;
    if (xy.y > y90+EPSLN || xy.y < -y90+EPSLN) /* 0 */
      z = 0;
    else if (xy.y >=  phi_boundary)
           if (xy.x <=  -d110) z =  1;
      else if (xy.x >=    d50) z =  3;
      else z = 2;
    else if (xy.y >=  0)
           if (xy.x <=  -d110) z =  4;
      else if (xy.x >=    d50) z =  6;
      else z = 5;
    else if (xy.y >= -phi_boundary) {
           if (xy.x <=   -d75) z =  7;
      else if (xy.x >=    d90) z =  9;
      else z = 8;
    }
    else {
           if (xy.x <=   -d75) z =  10;
      else if (xy.x >=    d90) z =  11;
      else z = 12;
    }

    if (z) {
        bool ok = false;

        xy.x -= Q->pj[z-1]->x0;
        xy.y -= Q->pj[z-1]->y0;
        lp = Q->pj[z-1]->inv(xy, Q->pj[z-1]);
        lp.lam += Q->pj[z-1]->lam0;

        switch (z) {
        /* Extend the plottable area slightly for each sub-projection */
        case  1:  ok = (lp.lam  >= -d180-EPSLN && lp.lam <= -d110+EPSLN) ||
                       ((lp.lam >= -d180-EPSLN && lp.lam <=  -d70+EPSLN) &&
                       (lp.phi  >=   d60-EPSLN && lp.phi <=   d90+EPSLN)); break;
        case  2:  ok = (lp.lam  >= -d110-EPSLN && lp.lam <=   d50+EPSLN) ||
                       ((lp.lam >=   d50-EPSLN && lp.lam <=   d60+EPSLN) &&
                       (lp.phi  >=   d60-EPSLN && lp.phi <=   d90+EPSLN)) ||
                       ((lp.lam <= -d120-EPSLN && lp.lam >= -d110+EPSLN) &&
                       (lp.phi  >=   d60-EPSLN && lp.phi <=   d90+EPSLN)) ; break;
        case  3:  ok = (lp.lam  >=   d50-EPSLN && lp.lam <=  d180+EPSLN) ||
                       ((lp.lam >=   d10-EPSLN && lp.lam <=   d60+EPSLN) &&
                       (lp.phi  >=   d60-EPSLN && lp.phi <=   d90+EPSLN)); break;
        case  4:  ok = (lp.lam  >= -d180-EPSLN && lp.lam <= -d110+EPSLN); break;
        case  5:  ok = (lp.lam  >= -d110-EPSLN && lp.lam <=   d50+EPSLN); break;
        case  6:  ok = (lp.lam  >=   d50-EPSLN && lp.lam <=  d180+EPSLN); break;
        case  7:  ok = (lp.lam  >= -d180-EPSLN && lp.lam <=  -d75+EPSLN); break;
        case  8:  ok = (lp.lam  >=  -d75-EPSLN && lp.lam <=   d90+EPSLN); break;
        case  9:  ok = (lp.lam  >=   d90-EPSLN && lp.lam <=  d180+EPSLN); break;
        case  10: ok = (lp.lam  >= -d180-EPSLN && lp.lam <=  -d75+EPSLN); break;
        case  11: ok = (lp.lam  >=  -d75-EPSLN && lp.lam <=   d90+EPSLN); break;
        case  12: ok = (lp.lam  >=   d90-EPSLN && lp.lam <=  d180+EPSLN); break;

        }
      z = (!ok? 0: z); /* projectable? */
    }

    if (!z) lp.lam = HUGE_VAL;
    if (!z) lp.phi = HUGE_VAL;

    return lp;
}


static PJ *destructor (PJ *P, int errlev) {
    int i;
    if (nullptr==P)
        return nullptr;

    if (nullptr==P->opaque)
        return pj_default_destructor (P, errlev);

    for (i = 0; i < 12; ++i) {
        if (static_cast<struct pj_opaque*>(P->opaque)->pj[i])
            static_cast<struct pj_opaque*>(P->opaque)->pj[i]->destructor(static_cast<struct pj_opaque*>(P->opaque)->pj[i], errlev);
    }

    return pj_default_destructor(P, errlev);
}



/*
  Zones:

    -180      -110               50           180
      +---------+----------------+-------------+    Zones 1,2,3,10,11 & 12:
      |1        |2               |3            |    Mollweide projection
      |         |                |             |
      +---------+----------------+-------------+    Zones 4,5,6,7,8 & 9:
      |4        |5               |6            |    Sinusoidal projection
      |         |                |             |
    0 +---------+--+-------------+--+----------+
      |7           |8               |9         |
      |            |                |          |
      +------------+----------------+----------+
      |10          |11              |12        |
      |            |                |          |
      +------------+----------------+----------+
    -180          -75               90        180
*/

static bool setup_zone(PJ *P, struct pj_opaque *Q, int n,
                       PJ*(*proj_ptr)(PJ*), double x_0,
                       double y_0, double lon_0) {
    if (!(Q->pj[n-1] = proj_ptr(nullptr))) return false;
    if (!(Q->pj[n-1] = proj_ptr(Q->pj[n-1]))) return false;
    Q->pj[n-1]->ctx = P->ctx;
    Q->pj[n-1]->x0 = x_0;
    Q->pj[n-1]->y0 = y_0;
    Q->pj[n-1]->lam0 = lon_0;
    return true;
}

PJ *PROJECTION(igh_o) {
    PJ_XY xy1, xy3;
    PJ_LP lp = { 0, phi_boundary };
    struct pj_opaque *Q = static_cast<struct pj_opaque*>(pj_calloc (1, sizeof (struct pj_opaque)));
    if (nullptr==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;


    /* sinusoidal zones */
    if (!setup_zone(P, Q, 4, pj_sinu, -d150, 0, -d150) ||
        !setup_zone(P, Q, 5, pj_sinu, -d40,  0,  -d40) ||
        !setup_zone(P, Q, 6, pj_sinu,  d120, 0,  d120) ||
        !setup_zone(P, Q, 7, pj_sinu, -d120, 0, -d120) ||
        !setup_zone(P, Q, 8, pj_sinu,   d10, 0,   d10) ||
        !setup_zone(P, Q, 9, pj_sinu,  d130, 0,  d130))
    {
       return destructor(P, ENOMEM);
    }


    /* mollweide zones */
    setup_zone(P, Q, 1, pj_moll,  -d150, 0,  -d150);

    /* y0 ? */
    xy1 = Q->pj[0]->fwd(lp, Q->pj[0]); /* zone 1 */
    xy3 = Q->pj[4]->fwd(lp, Q->pj[4]); /* zone 3 */
    /* y0 + xy1.y = xy3.y for lt = 40d44'11.8" */
    Q->dy0 = xy3.y - xy1.y;

    Q->pj[0]->y0 = Q->dy0;

    /* mollweide zones (cont'd) */
    if (!setup_zone(P, Q,  2, pj_moll,  -d40,  Q->dy0,  -d40)  ||
        !setup_zone(P, Q,  3, pj_moll,  d120,  Q->dy0,  d120)  ||
        !setup_zone(P, Q, 10, pj_moll, -d120, -Q->dy0, -d120)  ||
        !setup_zone(P, Q, 11, pj_moll,   d10, -Q->dy0,   d10)  ||
        !setup_zone(P, Q, 12, pj_moll,  d130, -Q->dy0,  d130))
    {
       return destructor(P, ENOMEM);
    }

    P->inv = igh_o_s_inverse;
    P->fwd = igh_o_s_forward;
    P->destructor = destructor;
    P->es = 0.;

    return P;
}
