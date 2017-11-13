/*

	Central Conic Projection (spherical only, tangent, gnomonic)
	
	Very rarely used, implemented for ATPOL Polish Geobotanical Grid

	see http://atpol.sourceforge.io

	Lukasz Komsta, 2017

*/


#define PJ_LIB__
#include <errno.h>
#include <proj.h>
#include "projects.h"

struct pj_opaque {
    double phi1;
    double ctgphi1;
    double sinphi1;
    double cosphi1;
    double *en;	
};

PROJ_HEAD(ccon, "Central Conic")
    "\n\tCentral Conic, Sph.\n\tlat_1=";



static XY e_forward (LP lp, PJ *P) {          
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double r;

    r = Q->ctgphi1 - tan(lp.phi - Q->phi1);
    xy.x = r * sin(lp.lam * Q->sinphi1);
    xy.y = Q->ctgphi1 - r * cos(lp.lam * Q->sinphi1);	

    return xy;
}


static LP e_inverse (XY xy, PJ *P) {         
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;

    xy.y = Q->ctgphi1 - xy.y;
    lp.phi = Q->phi1 - atan(hypot(xy.x,xy.y) - Q->ctgphi1);
    lp.lam = atan2(xy.x,xy.y)/Q->sinphi1;

    return lp;
}


static void *destructor (PJ *P, int errlev) {                       
    if (0==P)
        return 0;

    if (0==P->opaque)
        return pj_default_destructor (P, errlev);

    pj_dealloc (P->opaque->en);
    return pj_default_destructor (P, errlev);
}


PJ *PROJECTION(ccon) {

    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;
    P->destructor = destructor;

    Q->phi1 = pj_param(P->ctx, P->params, "rlat_1").f;

    if (!(Q->en = pj_enfn(P->es)))
        return pj_default_destructor(P, ENOMEM);

    Q->sinphi1 = sin(Q->phi1);
    Q->cosphi1 = cos(Q->phi1);
    Q->ctgphi1 = Q->cosphi1/Q->sinphi1;


    P->inv = e_inverse;
    P->fwd = e_forward;
    
    return P;
}


int pj_ccon_selftest (void) {return 10000;}
