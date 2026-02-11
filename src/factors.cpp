/* projection scale factors */

#include "proj.h"
#include "proj_internal.h"
#include <math.h>

#include <errno.h>

#ifndef DEFAULT_H
#define DEFAULT_H 1e-5 /* radian default for numeric h */
#endif

#define EPS 1.0e-12

static int pj_factors(PJ_LP lp, PJ *toplevel, const PJ *internal, double h,
                      struct FACTORS *fac) {
    double cosphi, t, n, r;
    int err;
    PJ_COORD coo = {{0, 0, 0, 0}};
    coo.lp = lp;

    if (HUGE_VAL == lp.lam)
        return 1;

    /* But from here, we're ready to make our own mistakes */
    err = proj_errno_reset(toplevel);

    /* Indicate that all factors are numerical approximations */
    fac->code = 0;

    /* Check for latitude or longitude overange */
    if ((fabs(lp.phi) - M_HALFPI) > EPS) {
        proj_log_error(toplevel, _("Invalid latitude"));
        proj_errno_set(toplevel, PROJ_ERR_COORD_TRANSFM_INVALID_COORD);
        return 1;
    }
    if (fabs(lp.lam) > 10.) {
        proj_log_error(toplevel, _("Invalid longitude"));
        proj_errno_set(toplevel, PROJ_ERR_COORD_TRANSFM_INVALID_COORD);
        return 1;
    }

    /* Set a reasonable step size for the numerical derivatives */
    h = fabs(h);
    if (h < EPS)
        h = DEFAULT_H;

    /* If input latitudes are geocentric, convert to geographic */
    if (internal->geoc)
        lp = pj_geocentric_latitude(internal, PJ_INV, coo).lp;

    /* If latitude + one step overshoots the pole, move it slightly inside, */
    /* so the numerical derivative still exists */
    if (fabs(lp.phi) > (M_HALFPI - h))
        lp.phi = lp.phi < 0. ? -(M_HALFPI - h) : (M_HALFPI - h);

    /* Longitudinal distance from central meridian */
    /* Only apply that correction for a non-pipeline transformation, otherwise
     * it will be applied twice (here and by the fwd_prepare() method of the
     * first step)
     */
    const bool bIsPipeline =
        internal->short_name && strcmp(internal->short_name, "pipeline") == 0;
    if (!bIsPipeline) {
        lp.lam -= internal->lam0;
        if (!internal->over)
            lp.lam = adjlon(lp.lam);
    }

    /* Derivatives */
    if (pj_deriv(lp, h, internal, &(fac->der))) {
        proj_log_error(toplevel, _("Invalid latitude or longitude"));
        proj_errno_set(toplevel, PROJ_ERR_COORD_TRANSFM_INVALID_COORD);
        return 1;
    }

    /* This code path is intended to be taken for a derived projected CRS,
     * with first step being a map projection, and second step some other
     * transformation. We must undo the scaling from the unit sphere to the
     * actual semi major radius done in the fwd_finalize() method of the
     * first step, otherwise most factors will be wrong.
     */
    if (bIsPipeline) {
        fac->der.x_l *= internal->ra * internal->fr_meter;
        fac->der.x_p *= internal->ra * internal->fr_meter;
        fac->der.y_l *= internal->ra * internal->fr_meter;
        fac->der.y_p *= internal->ra * internal->fr_meter;
    }

    /* Scale factors */
    cosphi = cos(lp.phi);
    fac->h = hypot(fac->der.x_p, fac->der.y_p);
    fac->k = hypot(fac->der.x_l, fac->der.y_l) / cosphi;

    if (internal->es != 0.0) {
        t = sin(lp.phi);
        t = 1. - internal->es * t * t;
        n = sqrt(t);
        fac->h *= t * n / internal->one_es;
        fac->k *= n;
        r = t * t / internal->one_es;
    } else
        r = 1.;

    /* Convergence */
    fac->conv = -atan2(fac->der.x_p, fac->der.y_p);

    /* Areal scale factor */
    fac->s = (fac->der.y_p * fac->der.x_l - fac->der.x_p * fac->der.y_l) * r /
             cosphi;

    /* Meridian-parallel angle (theta prime) */
    fac->thetap = aasin(internal->ctx, fac->s / (fac->h * fac->k));

    /* Tissot ellipse axis */
    t = fac->k * fac->k + fac->h * fac->h;
    fac->a = sqrt(t + 2. * fac->s);
    t = t - 2. * fac->s;
    t = t > 0 ? sqrt(t) : 0;
    fac->b = 0.5 * (fac->a - t);
    fac->a = 0.5 * (fac->a + t);

    /* Angular distortion */
    fac->omega =
        2. * aasin(internal->ctx, (fac->a - fac->b) / (fac->a + fac->b));

    proj_errno_restore(toplevel, err);
    return 0;
}

/*****************************************************************************/
PJ_FACTORS proj_factors(PJ *P, PJ_COORD lp) {
    /******************************************************************************
        Cartographic characteristics at point lp.

        Characteristics include meridian, parallel and areal scales, angular
        distortion, meridian/parallel, meridian convergence and scale error.

        returns PJ_FACTORS. If unsuccessful, error number is set and the
        struct returned contains NULL data.
    ******************************************************************************/
    PJ_FACTORS factors = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    struct FACTORS f;

    if (nullptr == P)
        return factors;

    auto pj = P;
    auto type = proj_get_type(pj);

    PJ *horiz = nullptr;
    if (pj->cached_op_for_proj_factors) {
        pj = pj->cached_op_for_proj_factors;
    } else {
        if (type == PJ_TYPE_COMPOUND_CRS) {
            horiz = proj_crs_get_sub_crs(pj->ctx, pj, 0);
            pj = horiz;
            type = proj_get_type(pj);
        }

        const auto createGeogCRSLonLatRad = [](PJ *base_geodetic_crs) {
            auto ctx = base_geodetic_crs->ctx;
            auto pm = proj_get_prime_meridian(ctx, base_geodetic_crs);
            double pm_longitude = 0;
            proj_prime_meridian_get_parameters(ctx, pm, &pm_longitude, nullptr,
                                               nullptr);
            proj_destroy(pm);
            PJ *geogCRSNormalized;
            auto cs = proj_create_ellipsoidal_2D_cs(
                ctx, PJ_ELLPS2D_LONGITUDE_LATITUDE, "Radian", 1.0);
            if (pm_longitude != 0) {
                auto ellipsoid = proj_get_ellipsoid(ctx, base_geodetic_crs);
                double semi_major_metre = 0;
                double inv_flattening = 0;
                proj_ellipsoid_get_parameters(ctx, ellipsoid, &semi_major_metre,
                                              nullptr, nullptr,
                                              &inv_flattening);
                geogCRSNormalized = proj_create_geographic_crs(
                    ctx, "unname crs", "unnamed datum",
                    proj_get_name(ellipsoid), semi_major_metre, inv_flattening,
                    "reference prime meridian", 0, nullptr, 0, cs);
                proj_destroy(ellipsoid);
            } else {
                auto datum = proj_crs_get_datum(ctx, base_geodetic_crs);
                auto datum_ensemble =
                    proj_crs_get_datum_ensemble(ctx, base_geodetic_crs);
                geogCRSNormalized = proj_create_geographic_crs_from_datum(
                    ctx, "unnamed crs", datum ? datum : datum_ensemble, cs);
                proj_destroy(datum);
                proj_destroy(datum_ensemble);
            }
            proj_destroy(cs);
            return geogCRSNormalized;
        };

        double semiMajorToDivide = 1;
        if (type == PJ_TYPE_PROJECTED_CRS) {
            // If it is a projected CRS, then compute the factors on the
            // conversion associated to it. We need to start from a temporary
            // geographic CRS using the same datum as the one of the projected
            // CRS, and with input coordinates being in longitude, latitude
            // order in radian, to be consistent with the expectations of the lp
            // input parameter. We also need to create a modified projected CRS
            // with a normalized easting/northing axis order in metre, so the
            // resulting operation is just a single step pipeline with no
            // axisswap or unitconvert steps.

            auto ctx = pj->ctx;
            auto geodetic_crs = proj_get_source_crs(ctx, pj);
            assert(geodetic_crs);
            auto geogCRSNormalized = createGeogCRSLonLatRad(geodetic_crs);
            auto conversion = proj_crs_get_coordoperation(ctx, pj);
            auto projCS = proj_create_cartesian_2D_cs(
                ctx, PJ_CART2D_EASTING_NORTHING, "metre", 1.0);
            auto projCRSNormalized = proj_create_projected_crs(
                ctx, nullptr, geodetic_crs, conversion, projCS);
            assert(projCRSNormalized);
            proj_destroy(geodetic_crs);
            proj_destroy(conversion);
            proj_destroy(projCS);
            auto newOp = proj_create_crs_to_crs_from_pj(
                ctx, geogCRSNormalized, projCRSNormalized, nullptr, nullptr);
            proj_destroy(geogCRSNormalized);
            proj_destroy(projCRSNormalized);
            assert(newOp);
            // For debugging:
            // printf("%s\n", proj_as_proj_string(ctx, newOp, PJ_PROJ_5,
            // nullptr));

            P->cached_op_for_proj_factors = newOp;
            pj = newOp;

        } else if (type == PJ_TYPE_DERIVED_PROJECTED_CRS) {

            auto ctx = pj->ctx;
            auto base_proj_crs = proj_get_source_crs(ctx, pj);
            assert(base_proj_crs);
            auto geodetic_crs = proj_get_source_crs(ctx, base_proj_crs);
            proj_destroy(base_proj_crs);
            assert(geodetic_crs);
            auto ellipsoid = proj_get_ellipsoid(ctx, geodetic_crs);
            proj_ellipsoid_get_parameters(ctx, ellipsoid, &semiMajorToDivide,
                                          nullptr, nullptr, nullptr);
            proj_destroy(ellipsoid);
            auto geogCRSNormalized = createGeogCRSLonLatRad(geodetic_crs);
            auto newOp = proj_create_crs_to_crs_from_pj(ctx, geogCRSNormalized,
                                                        pj, nullptr, nullptr);
            proj_destroy(geodetic_crs);
            proj_destroy(geogCRSNormalized);
            assert(newOp);

            PJ *cs = proj_crs_get_coordinate_system(ctx, pj);
            double cs_unit_conv_factor = 1.0;
            proj_cs_get_axis_info(ctx, cs, 0, nullptr, nullptr, nullptr,
                                  &cs_unit_conv_factor, nullptr, nullptr,
                                  nullptr);
            proj_destroy(cs);

            // Remove trailing unit conversion and axis swap
            std::string osProjStr =
                proj_as_proj_string(ctx, newOp, PJ_PROJ_5, nullptr);
            bool resized = false;
            for (const char *suffix :
                 {" +step +proj=unitconvert", " +step +proj=axiswap"}) {
                auto nPos = osProjStr.rfind(suffix);
                if (nPos != std::string::npos) {
                    resized = true;
                    osProjStr.resize(nPos);
                }
            }
            if (resized) {
                proj_destroy(newOp);
                newOp = proj_create(ctx, osProjStr.c_str());
                assert(newOp);
            }

            newOp->fr_meter = cs_unit_conv_factor;

            P->cached_op_for_proj_factors = newOp;
            pj = newOp;

        } else if (type != PJ_TYPE_CONVERSION &&
                   type != PJ_TYPE_TRANSFORMATION &&
                   type != PJ_TYPE_CONCATENATED_OPERATION &&
                   type != PJ_TYPE_OTHER_COORDINATE_OPERATION) {
            proj_log_error(P, _("Invalid type for P object"));
            proj_errno_set(P, PROJ_ERR_INVALID_OP_ILLEGAL_ARG_VALUE);
            if (horiz)
                proj_destroy(horiz);
            return factors;
        }
    }

    const int ret = pj_factors(lp.lp, P, pj, 0.0, &f);
    if (horiz)
        proj_destroy(horiz);
    if (ret)
        return factors;

    factors.meridional_scale = f.h;
    factors.parallel_scale = f.k;
    factors.areal_scale = f.s;

    factors.angular_distortion = f.omega;
    factors.meridian_parallel_angle = f.thetap;
    factors.meridian_convergence = f.conv;

    factors.tissot_semimajor = f.a;
    factors.tissot_semiminor = f.b;

    /* Raw derivatives, for completeness's sake */
    factors.dx_dlam = f.der.x_l;
    factors.dx_dphi = f.der.x_p;
    factors.dy_dlam = f.der.y_l;
    factors.dy_dphi = f.der.y_p;

    return factors;
}
