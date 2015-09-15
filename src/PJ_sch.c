#define PROJ_PARMS__ \
	double plat; \
        double plon; \
        double phdg; \
        double h0; \
	double transMat[3][3]; \
        double xyzoff[3]; \
        double rcurv; 
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(sch, "Spherical Cross-track Height") "\n\tMisc\n\tplat_0 = ,plon_0 = , phdg_0 = ,[h_0 = ]";

INVERSE3D(inverse3d);
        (void) P;
        lpz.lam = xyz.x;
        lpz.phi = xyz.y;
        lpz.z = xyz.z;
        return (lpz);
}

FORWARD3D(forward3d);
        (void) P;
        xyz.x = lpz.lam;
        xyz.y = lpz.phi;
        xyz.z = lpz.z;
        return (xyz);
}

FREEUP; if (P) pj_dalloc(P); }
	static PJ *
setup(PJ *P) { /* general initialization */
        P->fwd3d = forward3d;
        P->inv3d = inverse3d;
	return P;
}
ENTRY0(sch)
        P->h0 = 0.0;

        //Check if peg latitude was defined
        if (pj_param(P->ctx, P->params, "tplat_0").i)
            P->plat = pj_param(P->ctx, P->params, "rplat_0").f;
        else
            E_ERROR(-37);

        //Check if peg longitude was defined
        if (pj_param(P->ctx, P->params, "tplon_0").i)
            P->plon = pj_param(P->ctx, P->params, "rplon_0").f;
        else
            E_ERROR(-37);
        
        //Check if peg latitude is defined
        if (pj_param(P->ctx, P->params, "tphdg_0").i)
            P->phdg = pj_param(P->ctx, P->params, "rhdg_0").f;
        else
            E_ERROR(-37);
        

        //Check if average height was defined
        //If so read it in
        if (pj_param(P->ctx, P->params, "th_0").i)
            P->h0 = pj_param(P->ctx, P->params, "dh_0").f;

        //Completed reading in the projection parameters
        printf("PSA: Lat = %f Lon = %f Hdg = %f \n", P->plat, P->plon, P->phdg);

ENDENTRY(setup(P))
