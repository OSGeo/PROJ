/*
 * Implementation of meta interrupted projection for some pseudocylindrical
 *
 * Copyright (c) 2025 Javier Jimenez Shaw
 *
 */

#include <errno.h>
#include <math.h>

#include <iostream>
#include <numeric>

#include "proj.h"
#include "proj_internal.h"

#define FROM_PROJ_CPP
#include "proj/internal/internal.hpp"

C_NAMESPACE PJ *pj_cass(PJ *);
C_NAMESPACE PJ *pj_poly(PJ *);
C_NAMESPACE PJ *pj_sinu(PJ *);
C_NAMESPACE PJ *pj_tmerc(PJ *);

PROJ_HEAD(interrupted, "Interrupted") "\n\tSph&Ell";

namespace { // anonymous namespace
struct pj_interrupted_data {
    PJ *base_pj;
    std::vector<double> lam_borders;
    std::vector<double> lam_centers;
    std::vector<double> x_borders;
    std::vector<double> x_centers;
};
} // anonymous namespace

static PJ_XY interrupted_forward(PJ_LP lp, PJ *P) {
    PJ_XY xy = {0.0, 0.0};
    struct pj_interrupted_data *Q =
        static_cast<struct pj_interrupted_data *>(P->opaque);
    double center_lam = 0;
    double center_x = 0;
    for (size_t i = 0; i < Q->lam_borders.size(); i++) {
        if (lp.lam <= Q->lam_borders[i]) {
            center_lam = Q->lam_centers[i];
            center_x = Q->x_centers[i];
            break;
        }
    }
    lp.lam -= center_lam;
    xy = Q->base_pj->fwd(lp, Q->base_pj);
    xy.x += center_x;
    return xy;
}

static PJ_LP interrupted_inverse(PJ_XY xy, PJ *P) {
    PJ_LP lp = {0.0, 0.0};

    struct pj_interrupted_data *Q =
        static_cast<struct pj_interrupted_data *>(P->opaque);

    double center_lam = 0;
    double center_x = 0;
    double start_lam = 0;
    double end_lam = 0;
    for (size_t i = 0; i < Q->x_borders.size(); i++) {
        if (xy.x <= Q->x_borders[i]) {
            center_lam = Q->lam_centers[i];
            center_x = Q->x_centers[i];
            end_lam = (i + 1 != Q->x_borders.size()) ? Q->lam_borders[i] : M_PI;
            start_lam = i > 0 ? Q->lam_borders[i - 1] : -M_PI;
            break;
        }
    }
    xy.x -= center_x;
    lp = Q->base_pj->inv(xy, Q->base_pj);
    lp.lam += center_lam;
    if (lp.lam > end_lam || lp.lam < start_lam) {
        proj_errno_set(P, PROJ_ERR_COORD_TRANSFM_OUTSIDE_PROJECTION_DOMAIN);
        return lp;
    }

    return lp;
}

static PJ *interrupted_destructor(PJ *P, int errlev) { /* Destructor */
    if (nullptr == P)
        return nullptr;
    if (nullptr == P->opaque)
        return pj_default_destructor(P, errlev);
    proj_destroy(static_cast<struct pj_interrupted_data *>(P->opaque)->base_pj);

    return pj_default_destructor(P, errlev);
}

PJ *PJ_PROJECTION(interrupted) {
    struct pj_interrupted_data *Q = static_cast<struct pj_interrupted_data *>(
        calloc(1, sizeof(struct pj_interrupted_data)));
    if (nullptr == Q)
        return pj_default_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
    P->opaque = Q;
    P->destructor = interrupted_destructor;

    std::string base;
    if (pj_param(P->ctx, P->params, "tbase").i) {
        base = pj_param(P->ctx, P->params, "sbase").s;
    }
    if (base != "cass" && base != "poly" && base != "sinu" && base != "tmerc") {
        proj_log_error(P, _("Invalid value for base parameter"));
        return pj_default_destructor(P, PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
    }
    std::vector<double> sizes;
    if (pj_param(P->ctx, P->params, "tgores").i) {
        std::string g = pj_param(P->ctx, P->params, "sgores").s;
        if (g[0] == 'l' || g[0] == 'L') {
            // if the first char is not alpha, projinfo parses
            // completely as a number. Start with an L to allow the commas.
            g = g.substr(1);
        }
        std::vector<std::string> gores = osgeo::proj::internal::split(g, ",");
        bool success = true;
        for (const auto &gore : gores) {
            if (gore.empty())
                continue;
            double d = osgeo::proj::internal::c_locale_stod(gore, success);
            if (!success) {
                proj_log_error(P, _("Invalid value for gores parameter"));
                return pj_default_destructor(
                    P, PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
            }
            sizes.push_back(d);
        }
    } else {
        sizes.push_back(3.0);
    }

    if (sizes.size() == 1) {
        sizes = std::vector<double>(static_cast<size_t>(sizes.front()), 1.0);
    }
    if (sizes.empty()) {
        proj_log_error(P, _("Invalid 0 gores"));
        return pj_default_destructor(P, PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
    }
    for (size_t i = 0; i < sizes.size(); i++) {
        if (sizes[i] != sizes[sizes.size() - i - 1]) {
            proj_log_error(P, _("gores sizes must be symmetric"));
            return pj_default_destructor(P,
                                         PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
        }
    }

    auto get_fun_ptr = [&base]() {
        if (base == "cass") {
            return pj_cass;
        } else if (base == "poly") {
            return pj_poly;
        } else if (base == "sinu") {
            return pj_sinu;
        } else if (base == "tmerc") {
            return pj_tmerc;
        }
        return pj_sinu;
    };

    Q->base_pj = get_fun_ptr()(nullptr);
    if (Q->base_pj == nullptr)
        return interrupted_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
    Q->base_pj->ctx = P->ctx;
    Q->base_pj->es = P->es;
    Q->base_pj->one_es = P->one_es;
    Q->base_pj->k0 = P->k0;
    Q->base_pj->f = P->f;
    Q->base_pj->f2 = P->f2;
    Q->base_pj->n = P->n;
    Q->base_pj = get_fun_ptr()(Q->base_pj);
    if (Q->base_pj == nullptr)
        return interrupted_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);

    const double total_size = std::accumulate(sizes.begin(), sizes.end(), 0.0);

    double lam_start = -M_PI;
    std::vector<double> x_semi_widths;
    for (auto &s : sizes) {
        const double width = s * M_PI * 2 / total_size;
        auto x_wd = Q->base_pj->fwd({width / 2, 0}, Q->base_pj);
        x_semi_widths.push_back(x_wd.x);
        const double lam_end = lam_start + width;
        Q->lam_borders.push_back(lam_end);
        const double lam_center = (lam_start + lam_end) / 2;
        Q->lam_centers.push_back(lam_center);
        lam_start = lam_end;
    }
    double x_start =
        -std::accumulate(x_semi_widths.begin(), x_semi_widths.end(), 0.0);
    for (auto &x_semi_width : x_semi_widths) {
        const double x_center = x_start + x_semi_width;
        Q->x_centers.push_back(x_center);
        const double x_end = x_start + x_semi_width * 2.0;
        Q->x_borders.push_back(x_end);
        x_start = x_end;
    }
    Q->lam_borders.back() += 1.;
    Q->x_borders.back() += 100.;

    P->fwd = interrupted_forward;
    P->inv = interrupted_inverse;

    return P;
}
