

#include <math.h>

#include "proj.h"
#include "proj_internal.h"

PROJ_HEAD(putp2, "Putnins P2") "\n\tPCyl, Sph";

#define C_x 1.89490
#define C_y 1.71848
#define C_p 0.6141848493043784
#define EPS 1e-10
#define NITER 10
#define PI_DIV_3 1.0471975511965977

/* Close to the poles, where theta tends to pi/3 and the Newton iteration
 * converges only linearly: with theta = pi/3 - t and
 * F(theta) = theta + sin(theta) * (cos(theta) - 1), so that C_p = F(pi/3),
 * F(pi/3) - F(pi/3 - t) = t + cos(2pi/3 - t) * sin(t)
 *                         - 2 * cos(pi/3 - t/2) * sin(t/2) */
static double putp2_polar_d(double t) {
    return t + cos(2. * PI_DIV_3 - t) * sin(t) -
           2. * cos(PI_DIV_3 - 0.5 * t) * sin(0.5 * t);
}

static PJ_XY putp2_s_forward(PJ_LP lp, PJ *P) { /* Spheroidal, forward */
    PJ_XY xy = {0.0, 0.0};
    int i;
    (void)P;

    if (fabs(lp.phi) > M_HALFPI - 0.1) {
        /* solve F(pi/3) - F(pi/3 - t) = C_p * (1 - sin|phi|)
         *                             = 2 * C_p * sin(chi / 2)^2 */
        const double sh = sin(0.5 * (M_HALFPI - fabs(lp.phi)));
        const double w = 2. * C_p * sh * sh;
        double t = sqrt(w / (0.25 * sqrt(3.))); /* D(t) ~ sqrt(3)/4 * t^2 */
        for (i = 0; i < 30; ++i) {
            /* D'(t) = cos(theta) * (2 * cos(theta) - 1),
             * with 2 * cos(theta) - 1 = 4 * sin(pi/3 - t/2) * sin(t/2) */
            const double d =
                cos(PI_DIV_3 - t) * 4. * sin(PI_DIV_3 - 0.5 * t) * sin(0.5 * t);
            if (d == 0.)
                break;
            const double dt = (putp2_polar_d(t) - w) / d;
            t -= dt;
            if (t < 0.)
                t = 0.;
            /* D(t) carries a rounding error of about eps * t, so also stop on
             * an absolute step */
            if (fabs(dt) <= 1e-12 * t || fabs(dt) < 1e-15)
                break;
        }
        /* cos(theta) - 1/2 = 2 * sin(pi/3 - t/2) * sin(t/2) */
        xy.x = C_x * lp.lam * 2. * sin(PI_DIV_3 - 0.5 * t) * sin(0.5 * t);
        xy.y = copysign(C_y * sin(PI_DIV_3 - t), lp.phi);
        return xy;
    }

    const double p = C_p * sin(lp.phi);
    const double phi_pow_2 = lp.phi * lp.phi;
    lp.phi *= 0.615709 + phi_pow_2 * (0.00909953 + phi_pow_2 * 0.0046292);
    for (i = NITER; i; --i) {
        const double c = cos(lp.phi);
        const double s = sin(lp.phi);
        const double V =
            (lp.phi + s * (c - 1.) - p) / (1. + c * (c - 1.) - s * s);
        lp.phi -= V;
        if (fabs(V) < EPS)
            break;
    }
    if (!i)
        lp.phi = lp.phi < 0 ? -PI_DIV_3 : PI_DIV_3;
    xy.x = C_x * lp.lam * (cos(lp.phi) - 0.5);
    xy.y = C_y * sin(lp.phi);

    return xy;
}

static PJ_LP putp2_s_inverse(PJ_XY xy, PJ *P) { /* Spheroidal, inverse */
    PJ_LP lp = {0.0, 0.0};

    const double theta = aasin(P->ctx, xy.y / C_y);
    if (fabs(theta) > PI_DIV_3 - 0.5) {
        /* Close to the poles: theta = +/-(pi/3 - t), with the colatitude chi
         * from 2 * C_p * sin(chi / 2)^2 = F(pi/3) - F(pi/3 - t) */
        double t = PI_DIV_3 - fabs(theta);
        if (t < 0.)
            t = 0.;
        const double den = C_x * 2. * sin(PI_DIV_3 - 0.5 * t) * sin(0.5 * t);
        lp.lam = den == 0. ? 0. : xy.x / den;
        lp.phi = copysign(
            M_HALFPI - 2. * asin(sqrt(0.5 * putp2_polar_d(t) / C_p)), xy.y);
        return lp;
    }
    lp.phi = theta;
    const double c = cos(lp.phi);
    lp.lam = xy.x / (C_x * (c - 0.5));
    lp.phi = aasin(P->ctx, (lp.phi + sin(lp.phi) * (c - 1.)) / C_p);

    return lp;
}

PJ *PJ_PROJECTION(putp2) {
    P->es = 0.;
    P->inv = putp2_s_inverse;
    P->fwd = putp2_s_forward;

    return P;
}

#undef C_x
#undef C_y
#undef C_p
#undef EPS
#undef NITER
#undef PI_DIV_3
