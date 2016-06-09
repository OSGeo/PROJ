#define PJ_LIB__
#include <projects.h>

PROJ_HEAD(nav_merc, "Navionics Mercator") "\n\tEll\n\tlat_ts=";

#define EPS10 1.e-10
#define KC 1.00676429271698

        
static XY e_forward (LP lp, PJ *P) {          /* Ellipsoidal, forward */
    XY xy = {0.0,0.0};
    if (fabs(fabs(lp.phi) - M_HALFPI) <= EPS10)
        F_ERROR; 
   
    xy.y =  log(tan( .5 * atan (tan(lp.phi)/KC) + M_FORTPI) )  ; 
    xy.x = P->k0 * lp.lam;
    return (xy);
}

static LP e_inverse (XY xy, PJ *P) {          /* Ellipsoidal, inverse */
    LP lp = {0.0,0.0};
    if ((lp.phi = pj_phi2(P->ctx, exp(- xy.y / P->k0), P->e)) == HUGE_VAL)
        I_ERROR;

    lp.phi = atan2((tan((atan(exp(xy.y)) * 2.) - M_HALFPI) * KC), 1);
    lp.lam = xy.x / P->k0;
    return lp;
}

static void freeup(PJ *P) {                             /* Destructor */
    pj_dealloc(P);
}


PJ *PROJECTION(nav_merc) {
    double phits=0.0;
    int is_phits;

    if( (is_phits = pj_param(P->ctx, P->params, "tlat_ts").i) ) {
        phits = fabs(pj_param(P->ctx, P->params, "rlat_ts").f);
        if (phits >= M_HALFPI) E_ERROR(-24);
    }

    if (P->es) { /* ellipsoid */
        if (is_phits)
            P->k0 = pj_msfn(sin(phits), cos(phits), P->es);
        P->inv = e_inverse;
        P->fwd = e_forward;
    }

    return P;
}


#ifdef PJ_OMIT_SELFTEST
int pj_nav_merc_selftest (void) {return 0;}
#else

int pj_nav_merc_selftest (void) {
    double tolerance_lp = 1e-10;
    double tolerance_xy = 1e-7;

    char e_args[] = {"+proj=nav_merc +a=6378388.0 +rf=297"};

    LP fwd_in[] = {
        { 950594.539, 5968306.230},
        { 950594.539, 5968306.230},
        { 950594.539, 5968306.230},
        { 950594.539, 5968306.230}
    };

    XY e_fwd_expect[] = {
        { 8.5390000000, 47.3515666667},
        { 8.5390000000, 47.3515666667},
        { 8.5390000000, 47.3515666667},
        { 8.5390000000, 47.3515666667},
    };

    XY inv_in[] = {
        { 8.5390000000, 47.3515666667},
        { 8.5390000000, 47.3515666667},
        { 8.5390000000, 47.3515666667},
        { 8.5390000000, 47.3515666667}
    };

    LP e_inv_expect[] = {
        { 950594.539, 5968306.230},
        { 950594.539, 5968306.230},
        {950594.539, 5968306.230},
        {950594.539, 5968306.230},
    };

    
    return pj_generic_selftest (e_args, 0, tolerance_xy, tolerance_lp, 4, 4, fwd_in, 0, e_fwd_expect, inv_in, 0, e_inv_expect );
}


#endif
      
            
            
            
            
            
            
            
            
            
