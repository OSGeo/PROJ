

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"

PROJ_HEAD(moll, "Mollweide") "\n\tPCyl, Sph";
PROJ_HEAD(wag4, "Wagner IV") "\n\tPCyl, Sph";
PROJ_HEAD(wag5, "Wagner V") "\n\tPCyl, Sph";

#define MAX_ITER 30
#define LOOP_TOL 1e-7

namespace { // anonymous namespace
struct pj_moll_data {
    double C_x, C_y, C_p;
    bool mollweide; /* p = pi/2: the poles need the special handling below */
};
} // anonymous namespace

/* Mollweide close to the poles. With theta = (pi - s) / 2 and chi the
 * colatitude, 2 * theta + sin(2 * theta) = pi * sin(phi) becomes
 * s - sin(s) = 2 * pi * sin(chi / 2)^2, which can be solved accurately where
 * the Newton iteration on theta converges only linearly. */
static double mollweide_polar_s(double chi) {
    const double sh = sin(0.5 * chi);
    const double w = 2. * M_PI * sh * sh;
    double s = cbrt(6. * w); /* <= the root, as s - sin(s) <= s^3 / 6 */
    for (int i = 0; i < 20; ++i) {
        const double sh2 = sin(0.5 * s);
        const double d = 2. * sh2 * sh2; /* 1 - cos(s) */
        if (d == 0.)
            break;
        const double ds = (pj_s_minus_sin_s(s) - w) / d;
        s -= ds;
        if (fabs(ds) <= 1e-12 * s) /* quadratic convergence */
            break;
    }
    return s;
}

static PJ_XY moll_s_forward(PJ_LP lp, PJ *P) { /* Spheroidal, forward */
    PJ_XY xy = {0.0, 0.0};
    struct pj_moll_data *Q = static_cast<struct pj_moll_data *>(P->opaque);
    int i;

    if (Q->mollweide && fabs(lp.phi) > M_HALFPI - 0.5) {
        const double s = mollweide_polar_s(M_HALFPI - fabs(lp.phi));
        /* cos(theta) = sin(s / 2), sin(theta) = cos(s / 2) */
        xy.x = Q->C_x * lp.lam * sin(0.5 * s);
        xy.y = copysign(Q->C_y * cos(0.5 * s), lp.phi);
        return xy;
    }

    const double k = Q->C_p * sin(lp.phi);
    for (i = MAX_ITER; i; --i) {
        const double V = (lp.phi + sin(lp.phi) - k) / (1. + cos(lp.phi));
        lp.phi -= V;
        if (fabs(V) < LOOP_TOL)
            break;
    }
    if (!i)
        lp.phi = (lp.phi < 0.) ? -M_HALFPI : M_HALFPI;
    else
        lp.phi *= 0.5;
    xy.x = Q->C_x * lp.lam * cos(lp.phi);
    xy.y = Q->C_y * sin(lp.phi);
    return xy;
}

static PJ_LP moll_s_inverse(PJ_XY xy, PJ *P) { /* Spheroidal, inverse */
    PJ_LP lp = {0.0, 0.0};
    struct pj_moll_data *Q = static_cast<struct pj_moll_data *>(P->opaque);
    if (Q->mollweide && fabs(xy.y) > 0.5 * Q->C_y && fabs(xy.y) <= Q->C_y) {
        /* Close to the poles: cos(s / 2) = |y| / C_y, with s as in
         * mollweide_polar_s(), and the colatitude chi from
         * 2 * sin(chi / 2)^2 = (s - sin(s)) / pi */
        const double half_s =
            2. * asin(sqrt(0.5 * (Q->C_y - fabs(xy.y)) / Q->C_y));
        const double s = 2. * half_s;
        const double den = Q->C_x * sin(half_s);
        lp.lam = den == 0. ? 0. : xy.x / den; /* at the pole x is 0 */
        if (fabs(lp.lam) >= M_PI) {
            lp.lam = lp.phi = HUGE_VAL;
            return lp;
        }
        const double chi = 2. * asin(sqrt(pj_s_minus_sin_s(s) / (2. * M_PI)));
        lp.phi = copysign(M_HALFPI - chi, xy.y);
        return lp;
    }
    lp.phi = aasin(P->ctx, xy.y / Q->C_y);
    lp.lam = xy.x / (Q->C_x * cos(lp.phi));
    if (fabs(lp.lam) < M_PI) {
        lp.phi += lp.phi;
        lp.phi = aasin(P->ctx, (lp.phi + sin(lp.phi)) / Q->C_p);
    } else {
        lp.lam = lp.phi = HUGE_VAL;
    }
    return lp;
}

static PJ *setup(PJ *P, double p) {
    struct pj_moll_data *Q = static_cast<struct pj_moll_data *>(P->opaque);
    double r, sp, p2 = p + p;

    P->es = 0;
    sp = sin(p);
    r = sqrt(M_TWOPI * sp / (p2 + sin(p2)));

    Q->C_x = 2. * r / M_PI;
    Q->C_y = r / sp;
    Q->C_p = p2 + sin(p2);
    Q->mollweide = p == M_HALFPI;

    P->inv = moll_s_inverse;
    P->fwd = moll_s_forward;
    return P;
}

PJ *PJ_PROJECTION(moll) {
    struct pj_moll_data *Q = static_cast<struct pj_moll_data *>(
        calloc(1, sizeof(struct pj_moll_data)));
    if (nullptr == Q)
        return pj_default_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
    P->opaque = Q;

    return setup(P, M_HALFPI);
}

PJ *PJ_PROJECTION(wag4) {
    struct pj_moll_data *Q = static_cast<struct pj_moll_data *>(
        calloc(1, sizeof(struct pj_moll_data)));
    if (nullptr == Q)
        return pj_default_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
    P->opaque = Q;

    return setup(P, M_PI / 3.);
}

PJ *PJ_PROJECTION(wag5) {
    struct pj_moll_data *Q = static_cast<struct pj_moll_data *>(
        calloc(1, sizeof(struct pj_moll_data)));
    if (nullptr == Q)
        return pj_default_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
    P->opaque = Q;

    P->es = 0;
    Q->C_x = 0.90977;
    Q->C_y = 1.65014;
    Q->C_p = 3.00896;

    P->inv = moll_s_inverse;
    P->fwd = moll_s_forward;

    return P;
}

#undef MAX_ITER
#undef LOOP_TOL
