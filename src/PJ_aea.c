/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Implementation of the aea (Albers Equal Area) projection.
 *           and the leac (Lambert Equal Area Conic) projection
 * Author:   Gerald Evenden (1995)
 *           Thomas Knudsen (2016) - revise/add regression tests
 *
 ******************************************************************************
 * Copyright (c) 1995, Gerald Evenden
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

#define PJ_LIB__
#include "proj.h"
#include <errno.h>
#include "projects.h"
#include "proj_math.h"


# define EPS10  1.e-10
# define TOL7   1.e-7

PROJ_HEAD(aea, "Albers Equal Area") "\n\tConic Sph&Ell\n\tlat_1= lat_2=";
PROJ_HEAD(leac, "Lambert Equal Area Conic") "\n\tConic, Sph&Ell\n\tlat_1= south";


/* determine latitude angle phi-1 */
# define N_ITER 15
# define EPSILON 1.0e-7
# define TOL 1.0e-10
static double phi1_(double qs, double Te, double Tone_es) {
    int i;
    double Phi, sinpi, cospi, con, com, dphi;

    Phi = asin (.5 * qs);
    if (Te < EPSILON)
        return( Phi );
    i = N_ITER;
    do {
        sinpi = sin (Phi);
        cospi = cos (Phi);
        con = Te * sinpi;
        com = 1. - con * con;
        dphi = .5 * com * com / cospi * (qs / Tone_es -
           sinpi / com + .5 / Te * log ((1. - con) /
           (1. + con)));
        Phi += dphi;
    } while (fabs(dphi) > TOL && --i);
    return( i ? Phi : HUGE_VAL );
}


struct pj_opaque {
    double  ec;
    double  n;
    double  c;
    double  dd;
    double  n2;
    double  rho0;
    double  rho;
    double  phi1;
    double  phi2;
    double  *en;
    int     ellips;
};


static void *destructor (PJ *P, int errlev) {                        /* Destructor */
    if (0==P)
        return 0;

    if (0==P->opaque)
        return pj_default_destructor (P, errlev);

    pj_dealloc (P->opaque->en);
    return pj_default_destructor (P, errlev);
}





static XY e_forward (LP lp, PJ *P) {   /* Ellipsoid/spheroid, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    Q->rho = Q->c - (Q->ellips ? Q->n * pj_qsfn(sin(lp.phi), P->e, P->one_es) : Q->n2 * sin(lp.phi));;
    if (Q->rho < 0.) {
        proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
        return xy;
    }
    Q->rho = Q->dd * sqrt(Q->rho);
    xy.x = Q->rho * sin( lp.lam *= Q->n );
    xy.y = Q->rho0 - Q->rho * cos(lp.lam);
    return xy;
}


static LP e_inverse (XY xy, PJ *P) {   /* Ellipsoid/spheroid, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    if( (Q->rho = hypot(xy.x, xy.y = Q->rho0 - xy.y)) != 0.0 ) {
        if (Q->n < 0.) {
            Q->rho = -Q->rho;
            xy.x = -xy.x;
            xy.y = -xy.y;
        }
        lp.phi =  Q->rho / Q->dd;
        if (Q->ellips) {
            lp.phi = (Q->c - lp.phi * lp.phi) / Q->n;
            if (fabs(Q->ec - fabs(lp.phi)) > TOL7) {
                if ((lp.phi = phi1_(lp.phi, P->e, P->one_es)) == HUGE_VAL) {
                    proj_errno_set(P, PJD_ERR_TOLERANCE_CONDITION);
                    return lp;
                }
            } else
                lp.phi = lp.phi < 0. ? -M_HALFPI : M_HALFPI;
        } else if (fabs(lp.phi = (Q->c - lp.phi * lp.phi) / Q->n2) <= 1.)
            lp.phi = asin(lp.phi);
        else
            lp.phi = lp.phi < 0. ? -M_HALFPI : M_HALFPI;
        lp.lam = atan2(xy.x, xy.y) / Q->n;
    } else {
        lp.lam = 0.;
        lp.phi = Q->n > 0. ? M_HALFPI : - M_HALFPI;
    }
    return lp;
}



static PJ *setup(PJ *P) {
    double cosphi, sinphi;
    int secant;
    struct pj_opaque *Q = P->opaque;

    P->inv = e_inverse;
    P->fwd = e_forward;

    if (fabs(Q->phi1 + Q->phi2) < EPS10)
        return destructor(P, PJD_ERR_CONIC_LAT_EQUAL);
    Q->n = sinphi = sin(Q->phi1);
    cosphi = cos(Q->phi1);
    secant = fabs(Q->phi1 - Q->phi2) >= EPS10;
    if( (Q->ellips = (P->es > 0.))) {
        double ml1, m1;

        if (!(Q->en = pj_enfn(P->es)))
            return destructor(P, 0);
        m1 = pj_msfn(sinphi, cosphi, P->es);
        ml1 = pj_qsfn(sinphi, P->e, P->one_es);
        if (secant) { /* secant cone */
            double ml2, m2;

            sinphi = sin(Q->phi2);
            cosphi = cos(Q->phi2);
            m2 = pj_msfn(sinphi, cosphi, P->es);
            ml2 = pj_qsfn(sinphi, P->e, P->one_es);
            if (ml2 == ml1)
                return destructor(P, 0);

            Q->n = (m1 * m1 - m2 * m2) / (ml2 - ml1);
        }
        Q->ec = 1. - .5 * P->one_es * log((1. - P->e) /
            (1. + P->e)) / P->e;
        Q->c = m1 * m1 + Q->n * ml1;
        Q->dd = 1. / Q->n;
        Q->rho0 = Q->dd * sqrt(Q->c - Q->n * pj_qsfn(sin(P->phi0),
            P->e, P->one_es));
    } else {
        if (secant) Q->n = .5 * (Q->n + sin(Q->phi2));
        Q->n2 = Q->n + Q->n;
        Q->c = cosphi * cosphi + Q->n2 * sinphi;
        Q->dd = 1. / Q->n;
        Q->rho0 = Q->dd * sqrt(Q->c - Q->n2 * sin(P->phi0));
    }

    return P;
}


PJ *PROJECTION(aea) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;
    P->destructor = destructor;

    Q->phi1 = pj_param(P->ctx, P->params, "rlat_1").f;
    Q->phi2 = pj_param(P->ctx, P->params, "rlat_2").f;
    return setup(P);
}


PJ *PROJECTION(leac) {
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;
    P->destructor = destructor;

    Q->phi2 = pj_param(P->ctx, P->params, "rlat_1").f;
    Q->phi1 = pj_param(P->ctx, P->params, "bsouth").i ? - M_HALFPI: M_HALFPI;
    return setup(P);
}

