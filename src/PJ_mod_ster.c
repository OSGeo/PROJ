/* based upon Snyder and Linck, USGS-NMD */
#define PJ_LIB__
#include <projects.h>

PROJ_HEAD(mil_os, "Miller Oblated Stereographic") "\n\tAzi(mod)";
PROJ_HEAD(lee_os, "Lee Oblated Stereographic") "\n\tAzi(mod)";
PROJ_HEAD(gs48, "Mod. Stererographics of 48 U.S.") "\n\tAzi(mod)";
PROJ_HEAD(alsk, "Mod. Stererographics of Alaska") "\n\tAzi(mod)";
PROJ_HEAD(gs50, "Mod. Stererographics of 50 U.S.") "\n\tAzi(mod)";

#define EPSLN 1e-12

struct pj_opaque {
    COMPLEX *zcoeff; \
    double  cchio, schio; \
    int     n;
};


static XY e_forward (LP lp, PJ *P) {          /* Ellipsoidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    double sinlon, coslon, esphi, chi, schi, cchi, s;
    COMPLEX p;

    sinlon = sin(lp.lam);
    coslon = cos(lp.lam);
    esphi = P->e * sin(lp.phi);
    chi = 2. * atan(tan((M_HALFPI + lp.phi) * .5) *
        pow((1. - esphi) / (1. + esphi), P->e * .5)) - M_HALFPI;
    schi = sin(chi);
    cchi = cos(chi);
    s = 2. / (1. + Q->schio * schi + Q->cchio * cchi * coslon);
    p.r = s * cchi * sinlon;
    p.i = s * (Q->cchio * schi - Q->schio * cchi * coslon);
    p = pj_zpoly1(p, Q->zcoeff, Q->n);
    xy.x = p.r;
    xy.y = p.i;

    return xy;
}


static LP e_inverse (XY xy, PJ *P) {          /* Ellipsoidal, inverse */
    LP lp = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    int nn;
    COMPLEX p, fxy, fpxy, dp;
    double den, rh, z, sinz, cosz, chi, phi, dphi, esphi;

    p.r = xy.x;
    p.i = xy.y;
    for (nn = 20; nn ;--nn) {
        fxy = pj_zpolyd1(p, Q->zcoeff, Q->n, &fpxy);
        fxy.r -= xy.x;
        fxy.i -= xy.y;
        den = fpxy.r * fpxy.r + fpxy.i * fpxy.i;
        dp.r = -(fxy.r * fpxy.r + fxy.i * fpxy.i) / den;
        dp.i = -(fxy.i * fpxy.r - fxy.r * fpxy.i) / den;
        p.r += dp.r;
        p.i += dp.i;
        if ((fabs(dp.r) + fabs(dp.i)) <= EPSLN)
            break;
    }
    if (nn) {
        rh = hypot(p.r, p.i);
        z = 2. * atan(.5 * rh);
        sinz = sin(z);
        cosz = cos(z);
        lp.lam = P->lam0;
        if (fabs(rh) <= EPSLN) {
            lp.phi = P->phi0;
            return lp;
        }
        chi = aasin(P->ctx, cosz * Q->schio + p.i * sinz * Q->cchio / rh);
        phi = chi;
        for (nn = 20; nn ;--nn) {
            esphi = P->e * sin(phi);
            dphi = 2. * atan(tan((M_HALFPI + chi) * .5) *
                pow((1. + esphi) / (1. - esphi), P->e * .5)) - M_HALFPI - phi;
            phi += dphi;
            if (fabs(dphi) <= EPSLN)
                break;
        }
    }
    if (nn) {
        lp.phi = phi;
        lp.lam = atan2(p.r * sinz, rh * Q->cchio * cosz - p.i *
            Q->schio * sinz);
    } else
        lp.lam = lp.phi = HUGE_VAL;
    return lp;
}


static void *freeup_new (PJ *P) {                       /* Destructor */
    if (0==P)
        return 0;
    if (0==P->opaque)
        return pj_dealloc (P);

    pj_dealloc (P->opaque);
    return pj_dealloc(P);
}

static void freeup (PJ *P) {
    freeup_new (P);
    return;
}

static PJ *setup(PJ *P) { /* general initialization */
    struct pj_opaque *Q = P->opaque;
    double esphi, chio;

    if (P->es) {
        esphi = P->e * sin(P->phi0);
        chio = 2. * atan(tan((M_HALFPI + P->phi0) * .5) *
            pow((1. - esphi) / (1. + esphi), P->e * .5)) - M_HALFPI;
    } else
        chio = P->phi0;
    Q->schio = sin(chio);
    Q->cchio = cos(chio);
    P->inv = e_inverse;
    P->fwd = e_forward;

    return P;
}


/* Miller Oblated Stereographic */
PJ *PROJECTION(mil_os) {
    static COMPLEX AB[] = {
        {0.924500, 0.},
        {0.,       0.},
        {0.019430, 0.}
    };

    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return freeup_new (P);
    P->opaque = Q;

    Q->n = 2;
    P->lam0 = DEG_TO_RAD * 20.;
    P->phi0 = DEG_TO_RAD * 18.;
    Q->zcoeff = AB;
    P->es = 0.;

    return setup(P);
}


/* Lee Oblated Stereographic */
PJ *PROJECTION(lee_os) {
    static COMPLEX AB[] = {
        {0.721316,    0.},
        {0.,          0.},
        {-0.0088162, -0.00617325}
    };

    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return freeup_new (P);
    P->opaque = Q;

    Q->n = 2;
    P->lam0 = DEG_TO_RAD * -165.;
    P->phi0 = DEG_TO_RAD * -10.;
    Q->zcoeff = AB;
    P->es = 0.;

   return setup(P);
}


PJ *PROJECTION(gs48) {
    static COMPLEX /* 48 United States */
    AB[] = {
        {0.98879,   0.},
        {0.,        0.},
        {-0.050909, 0.},
        {0.,        0.},
        {0.075528,  0.}
    };

    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return freeup_new (P);
    P->opaque = Q;

    Q->n = 4;
    P->lam0 = DEG_TO_RAD * -96.;
    P->phi0 = DEG_TO_RAD * -39.;
    Q->zcoeff = AB;
    P->es = 0.;
    P->a = 6370997.;

    return setup(P);
}


PJ *PROJECTION(alsk) {
    static COMPLEX  ABe[] = { /* Alaska ellipsoid */
        { .9945303, 0.},
        { .0052083, -.0027404},
        { .0072721,  .0048181},
        {-.0151089, -.1932526},
        { .0642675, -.1381226},
        { .3582802, -.2884586},
    };

    static COMPLEX ABs[] = { /* Alaska sphere */
        { .9972523, 0.},
        { .0052513, -.0041175},
        { .0074606,  .0048125},
        {-.0153783, -.1968253},
        { .0636871, -.1408027},
        { .3660976, -.2937382}
    };

    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return freeup_new (P);
    P->opaque = Q;

    Q->n = 5;
    P->lam0 = DEG_TO_RAD * -152.;
    P->phi0 = DEG_TO_RAD * 64.;
    if (P->es) { /* fixed ellipsoid/sphere */
        Q->zcoeff = ABe;
        P->a = 6378206.4;
        P->e = sqrt(P->es = 0.00676866);
    } else {
        Q->zcoeff = ABs;
        P->a = 6370997.;
    }

    return setup(P);
}


PJ *PROJECTION(gs50) {
    static COMPLEX  ABe[] = { /* GS50 ellipsoid */
        { .9827497, 0.},
        { .0210669,  .0053804},
        {-.1031415, -.0571664},
        {-.0323337, -.0322847},
        { .0502303,  .1211983},
        { .0251805,  .0895678},
        {-.0012315, -.1416121},
        { .0072202, -.1317091},
        {-.0194029,  .0759677},
        {-.0210072,  .0834037}
    };

    static COMPLEX ABs[] = { /* GS50 sphere */
        { .9842990, 0.},
        { .0211642,  .0037608},
        {-.1036018, -.0575102},
        {-.0329095, -.0320119},
        { .0499471,  .1223335},
        { .0260460,  .0899805},
        { .0007388, -.1435792},
        { .0075848, -.1334108},
        {-.0216473,  .0776645},
        {-.0225161,  .0853673}
    };

    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return freeup_new (P);
    P->opaque = Q;

    Q->n = 9;
    P->lam0 = DEG_TO_RAD * -120.;
    P->phi0 = DEG_TO_RAD * 45.;
    if (P->es) { /* fixed ellipsoid/sphere */
        Q->zcoeff = ABe;
        P->a = 6378206.4;
        P->e = sqrt(P->es = 0.00676866);
    } else {
        Q->zcoeff = ABs;
        P->a = 6370997.;
    }

    return setup(P);
}


#ifdef PJ_OMIT_SELFTEST
int pj_mil_os_selftest (void) {return 0;}
#else

int pj_mil_os_selftest (void) {
    double tolerance_lp = 1e-10;
    double tolerance_xy = 1e-7;

    char s_args[] = {"+proj=mil_os   +a=6400000    +lat_1=0.5 +lat_2=2"};

    LP fwd_in[] = {
        { 2, 1},
        { 2,-1},
        {-2, 1},
        {-2,-1}
    };

    XY s_fwd_expect[] = {
        {-1908527.94959420455, -1726237.4730614475},
        {-1916673.02291848511, -1943133.88812552323},
        {-2344429.41208962305, -1706258.05121891224},
        {-2354637.83553299867, -1926468.60513541684},
    };

    XY inv_in[] = {
        { 200, 100},
        { 200,-100},
        {-200, 100},
        {-200,-100}
    };

    LP s_inv_expect[] = {
        {20.0020363939492398, 18.0009683469140498},
        {20.0020363715837419, 17.999031631815086},
        {19.9979636060507602, 18.0009683469140498},
        {19.9979636284162581, 17.999031631815086},
    };

    return pj_generic_selftest (0, s_args, tolerance_xy, tolerance_lp, 4, 4, fwd_in, 0, s_fwd_expect, inv_in, 0, s_inv_expect);
}

#endif


#ifdef PJ_OMIT_SELFTEST
int pj_lee_os_selftest (void) {return 0;}
#else

int pj_lee_os_selftest (void) {
    double tolerance_lp = 1e-10;
    double tolerance_xy = 1e-7;

    char s_args[] = {"+proj=lee_os   +a=6400000    +lat_1=0.5 +lat_2=2"};

    LP fwd_in[] = {
        { 2, 1},
        { 2,-1},
        {-2, 1},
        {-2,-1}
    };

    XY s_fwd_expect[] = {
        {-25564478.9526050538, 154490848.8286255},
        { 30115393.9385746419, 125193997.439701974},
        {-31039340.5921660066,  57678685.0448915437},
        {-3088419.93942357088,  58150091.0991110131},
    };

    XY inv_in[] = {
        { 200, 100},
        { 200,-100},
        {-200, 100},
        {-200,-100}
    };

    LP s_inv_expect[] = {
        {-164.997479457813824,  -9.99875886103541411},
        {-164.997479438558884, -10.0012411200022751},
        {-165.002520542186289,  -9.99875886103545142},
        {-165.002520561440946, -10.0012411200022999},
    };

    return pj_generic_selftest (0, s_args, tolerance_xy, tolerance_lp, 4, 4, fwd_in, 0, s_fwd_expect, inv_in, 0, s_inv_expect);
}

#endif


#ifdef PJ_OMIT_SELFTEST
int pj_gs48_selftest (void) {return 0;}
#else

int pj_gs48_selftest (void) {
    double tolerance_lp = 1e-10;
    double tolerance_xy = 1e-7;

    char s_args[] = {"+proj=gs48   +a=6400000    +lat_1=0.5 +lat_2=2"};

    LP fwd_in[] = {
        { 2, 1},
        { 2,-1},
        {-2, 1},
        {-2,-1}
    };

    XY s_fwd_expect[] = {
        {36625944.1923860237, -10443454.370062707},
        {32392147.7449533679, -12580705.358382076},
        {31394918.3206626177,  -3352580.92795714363},
        {28553241.7702435851,  -5673083.97367164213},
    };

    XY inv_in[] = {
        { 200, 100},
        { 200,-100},
        {-200, 100},
        {-200,-100}
    };

    LP s_inv_expect[] = {
        {-95.997669975773789,  -38.9990945807843801},
        {-95.9976699161415326, -39.0009053728726585},
        {-96.0023300242262252, -38.9990945807843801},
        {-96.0023300838584817, -39.0009053728726585},
    };

    return pj_generic_selftest (0, s_args, tolerance_xy, tolerance_lp, 4, 4, fwd_in, 0, s_fwd_expect, inv_in, 0, s_inv_expect);
}

#endif


#ifdef PJ_OMIT_SELFTEST
int pj_alsk_selftest (void) {return 0;}
#else

int pj_alsk_selftest (void) {

    /* The standard test points are way outside the definition area bounds, hence we relax tolerances */
    double tolerance_lp = 1e-7;
    double tolerance_xy = 1e-4;

    char e_args[] = {"+proj=alsk   +ellps=GRS80  +lat_1=0.5 +lat_2=2"};
    char s_args[] = {"+proj=alsk   +a=6400000    +lat_1=0.5 +lat_2=2"};

    LP fwd_in[] = {
        { 2, 1},
        { 2,-1},
        {-2, 1},
        {-2,-1}
    };

    XY e_fwd_expect[] = {
        {1642864080.02732754, -1139302009.02887797},
        {1991193850.2052319,  -1518851027.13339996},
        { 896601956.44416213, -1580143099.4169271},
        {1018553822.30535674, -2028874099.61853552},
    };

    XY s_fwd_expect[] = {
        {1651105846.06753755,  -1132233885.29928017},
        {2004822703.03845358,  -1512578241.93150067},
        { 907039096.196665168, -1578565868.60405397},
        {1033243931.08174837,  -2030485879.68075895},
    };

    XY inv_in[] = {
        { 200, 100},
        { 200,-100},
        {-200, 100},
        {-200,-100}
    };

    LP e_inv_expect[] = {
        {-151.99590140340095,  64.0008994430213534},
        {-151.995901665843263, 63.9991004415502687},
        {-152.004098597614217, 64.0008994436154524},
        {-152.004098335171818, 63.9991004409561413},
    };

    LP s_inv_expect[] = {
        {-151.995904192970613, 64.0008976554005926},
        {-151.995904455051942, 63.9991022295943921},
        {-152.004095808043218, 64.0008976559930716},
        {-152.004095545961775, 63.9991022290018208},
    };

    return pj_generic_selftest (e_args, s_args, tolerance_xy, tolerance_lp, 4, 4, fwd_in, e_fwd_expect, s_fwd_expect, inv_in, e_inv_expect, s_inv_expect);
}

#endif


#ifdef PJ_OMIT_SELFTEST
int pj_gs50_selftest (void) {return 0;}
#else

int pj_gs50_selftest (void) {
    double tolerance_lp = 1e-10;
    double tolerance_xy = 1e-7;

    char e_args[] = {"+proj=gs50   +ellps=GRS80  +lat_1=0.5 +lat_2=2"};
    char s_args[] = {"+proj=gs50   +a=6400000    +lat_1=0.5 +lat_2=2"};

    LP fwd_in[] = {
        { 2, 1},
        { 2,-1},
        {-2, 1},
        {-2,-1}
    };

    XY e_fwd_expect[] = {
        {29729016747.3571701,  -3918216218.3354063},
        {36735969814.5893631, -14677374776.9809761},
        {14161556831.8443203, -11410194658.0201168},
        {14213883663.1140423, -19065362387.8745575},
    };

    XY s_fwd_expect[] = {
        {30210185088.2270584, -3211636634.93554688},
        {37664745793.3519745, -13981519124.6516781},
        {14657726636.8231983, -11238369787.8965397},
        {14923510723.7682972, -19062616309.6883698},
    };

    XY inv_in[] = {
        { 200, 100},
        { 200,-100},
        {-200, 100},
        {-200,-100}
    };

    LP e_inv_expect[] = {
        {-119.997423271634844, 45.0009140704890811},
        {-119.997423354194112, 44.9990858708950014},
        {-120.00257673100937,  45.0009140729899428},
        {-120.002576648448965, 44.9990858683939408},
    };

    LP s_inv_expect[] = {
        {-119.997427429220934, 45.0009094967551704},
        {-119.997427511518453, 44.9990904451617482},
        {-120.002572573413815, 45.0009094992385954},
        {-120.00257249111516,  44.9990904426780602},
    };

    return pj_generic_selftest (e_args, s_args, tolerance_xy, tolerance_lp, 4, 4, fwd_in, e_fwd_expect, s_fwd_expect, inv_in, e_inv_expect, s_inv_expect);
}


#endif
