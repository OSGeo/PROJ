
#include <errno.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"
#include <iostream>
#include <math.h>

PROJ_HEAD(hourglass, "Hourglass") "\n\tSph, Ellips";

namespace { // anonymous namespace
struct pj_hourglass_data {
    int outinvalid = 0;
    double n = 0;
    double np1 = 0;
    double s = 0;
    double sqrts = 0;
    double L = 0;
    double L_nm1_n = 0;
    double L_1mn = 0;
    // for authalic latitudes:
    double qp = 0;
    double *apa = nullptr;
};
} // anonymous namespace

static PJ_XY hourglass_forward(PJ_LP lp, PJ *P) { /* Ellipsoidal, forward */
    PJ_XY xy = {0.0, 0.0};
    struct pj_hourglass_data *Q =
        static_cast<struct pj_hourglass_data *>(P->opaque);

    const double beta = Q->apa ? pj_authalic_lat(lp.phi, sin(lp.phi),
                                                 cos(lp.phi), Q->apa, P, Q->qp)
                               : lp.phi;

    const double sign = lp.phi >= 0 ? 1.0 : -1.0;
    const double n = Q->n;
    const double np1 = Q->np1;
    const double q = Q->qp * fabs(sin(beta));
    xy.x = lp.lam / Q->sqrts / M_PI *
           std::pow(M_PI_2 * np1 / n * std::pow(Q->L, n - 1) * q, 1 / np1);
    xy.y =
        sign * Q->sqrts *
        std::pow(M_PI_2 * np1 / n * std::pow(Q->L, (1 - n) / n) * q, n / np1);
    return xy;
}

static PJ_LP hourglass_inverse(PJ_XY xy, PJ *P) { /* Ellipsoidal, inverse */
    PJ_LP lp = {0.0, 0.0};

    if (std::fabs(xy.y) <= 1e-8) {
        // singularity at 0
        return lp;
    }

    struct pj_hourglass_data *Q =
        static_cast<struct pj_hourglass_data *>(P->opaque);

    const double sign = xy.y >= 0 ? 1.0 : -1.0;
    const double t = std::pow(std::fabs(xy.y) / Q->sqrts, Q->np1 / Q->n);
    const double sinbeta = t * Q->n / Q->np1 / M_PI * 2 / Q->qp * Q->L_nm1_n;
    if (sinbeta > 1) {
        proj_errno_set(P, PROJ_ERR_COORD_TRANSFM_OUTSIDE_PROJECTION_DOMAIN);
        return lp;
    }

    const double u = Q->n / Q->np1 / M_PI * 2 / Q->qp * Q->L_1mn / sinbeta;
    lp.lam = xy.x * M_PI * Q->sqrts * std::pow(u, 1 / Q->np1);
    const double beta = asin(sinbeta);
    lp.phi = Q->apa ? pj_authalic_lat_inverse(beta, Q->apa, P, Q->qp) : beta;
    lp.phi *= sign;

    if (Q->outinvalid &&
        (fabs(lp.lam) > (Q->outinvalid * 2 - 1) * M_PI + 1.e-7)) {
        proj_errno_set(P, PROJ_ERR_COORD_TRANSFM_OUTSIDE_PROJECTION_DOMAIN);
        return lp;
    }

    return lp;
}

static PJ *pj_hourglass_destructor(PJ *P, int errlev) {
    if (nullptr == P)
        return nullptr;

    if (nullptr == P->opaque)
        return pj_default_destructor(P, errlev);

    free(static_cast<struct pj_hourglass_data *>(P->opaque)->apa);

    return pj_default_destructor(P, errlev);
}

PJ *PJ_PROJECTION(hourglass) {
    struct pj_hourglass_data *Q = static_cast<struct pj_hourglass_data *>(
        calloc(1, sizeof(struct pj_hourglass_data)));
    if (nullptr == Q)
        return pj_hourglass_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
    P->opaque = Q;
    Q->outinvalid = 1;
    if (pj_param(P->ctx, P->params, "toutinvalid").i) {
        Q->outinvalid = pj_param(P->ctx, P->params, "ioutinvalid").i;
    }
    double s = 1;
    if (pj_param(P->ctx, P->params, "ts").i) {
        s = pj_param(P->ctx, P->params, "ds").f;
        if (s <= 0) {
            proj_log_error(P, _("Invalid value for ratio s should be > 0"));
            return pj_hourglass_destructor(
                P, PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
        }
    }
    double n = 1;
    if (pj_param(P->ctx, P->params, "tn").i) {
        n = pj_param(P->ctx, P->params, "dn").f;
        if (n <= 0) {
            proj_log_error(P, _("Invalid value for n: it should be > 0"));
            return pj_hourglass_destructor(
                P, PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
        }
    }
    const double np1 = n + 1;
    if (P->e != 0.0) {
        Q->L = std::sqrt(M_PI_2 * np1 / n *
                         (1 + (1 - P->es) / P->e * atanh(P->e)));
        Q->qp = pj_authalic_lat_q(1.0, P);
        Q->apa = pj_authalic_lat_compute_coeffs(P->n);
        if (nullptr == Q->apa)
            return pj_hourglass_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
    } else {
        Q->L = std::sqrt(M_PI * np1 / n);
        Q->qp = 2.0;
        Q->apa = nullptr;
    }
    Q->n = n;
    Q->np1 = np1;
    Q->L_nm1_n = std::pow(Q->L, (n - 1) / n);
    Q->L_1mn = std::pow(Q->L, 1 - n);
    Q->s = s;
    Q->sqrts = std::sqrt(s);
    P->inv = hourglass_inverse;
    P->fwd = hourglass_forward;
    return P;
}
