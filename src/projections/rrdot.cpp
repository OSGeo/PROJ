#include <errno.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"
#include <limits>

#define MAX_ITER 20

namespace { // anonymous namespace
struct pj_rrdot_data {
    PJ_XYZ pxyz; /* Platform position */
    PJ_XYZ vxyz; /* Platform velocity */
    int side;    /* Right = -1, Left = 1*/
    PJ *cart;
    PJ_XYZ that;
    PJ_XYZ chat;
    PJ_XYZ nhat;
    double dist;
    double radius;
    double height;
    double ndotv_vdott;
    double vmag;
};
} // anonymous namespace

#define RNG_TOL 1.0e-8

PROJ_HEAD(rrdot, "Range Doppler coordinates")
"\n\tMisc\n\tx= y= z= vx= vy= vz= left";

static PJ_LPZ rrdot_inverse3d(PJ_XYZ xyz, PJ *P) {
    struct pj_rrdot_data *Q = static_cast<struct pj_rrdot_data *>(P->opaque);

    PJ_XYZ pos;

    /* Slant range has to be positive*/
    if (xyz.x <= 0.) {
        proj_errno_set(P, PROJ_ERR_COORD_TRANSFM_OUTSIDE_PROJECTION_DOMAIN);
        PJ_LPZ lpz = {std::numeric_limits<double>::quiet_NaN(),
                      std::numeric_limits<double>::quiet_NaN(),
                      std::numeric_limits<double>::quiet_NaN()};
        return lpz;
    }
    xyz.x *= P->a;
    xyz.y *= P->a;

    /* This is based on isce-framework rdr2geo algorithm */
    bool converged = false;
    double h = xyz.z;

    for (int i = 0; i < MAX_ITER; i++) {
        /* Slant range has to be positive and greater than relative height of
         * platform */
        if ((Q->height - h) >= xyz.x) {
            proj_errno_set(P, PROJ_ERR_COORD_TRANSFM_OUTSIDE_PROJECTION_DOMAIN);
            PJ_LPZ lpz = {std::numeric_limits<double>::quiet_NaN(),
                          std::numeric_limits<double>::quiet_NaN(),
                          std::numeric_limits<double>::quiet_NaN()};
            return lpz;
        }
        double a = Q->dist;
        double b = Q->radius + h;
        double cos_theta =
            0.5 * (a / xyz.x + xyz.x / a - (b / a) * (b / xyz.x));
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        /* Compute TCN scale factors */
        double gamma = xyz.x * cos_theta;
        double alpha = (-xyz.x * xyz.y / Q->vmag) - gamma * Q->ndotv_vdott;
        a = xyz.x * sin_theta;
        double beta = -Q->side * sqrt(a * a - alpha * alpha);

        /* Compute target on ground */
        PJ_XYZ tvec = {Q->pxyz.x + alpha * Q->that.x + beta * Q->chat.x +
                           gamma * Q->nhat.x,
                       Q->pxyz.y + alpha * Q->that.y + beta * Q->chat.y +
                           gamma * Q->nhat.y,
                       Q->pxyz.z + alpha * Q->that.z + beta * Q->chat.z +
                           gamma * Q->nhat.z};

        /* Convert to llh */
        PJ_LPZ tllh = Q->cart->inv3d(tvec, Q->cart);

        /* Adjust height */
        tllh.z = xyz.z;
        pos = Q->cart->fwd3d(tllh, Q->cart);
        a = sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
        h = a - Q->radius;

        /* Height for next iteration */
        tvec.x = pos.x - Q->pxyz.x;
        tvec.y = pos.y - Q->pxyz.y;
        tvec.z = pos.z - Q->pxyz.z;
        a = sqrt(tvec.x * tvec.x + tvec.y * tvec.y + tvec.z * tvec.z);

        if (fabs(a - xyz.x) < RNG_TOL) {
            converged = true;
            break;
        }
    }

    if (!converged) {
        proj_errno_set(P, PROJ_ERR_COORD_TRANSFM_NO_CONVERGENCE);
    }

    /* Convert geocentric coordinates to lat long */
    return Q->cart->inv3d(pos, Q->cart);
}

static PJ_XYZ rrdot_forward3d(PJ_LPZ lpz, PJ *P) {
    struct pj_rrdot_data *Q = static_cast<struct pj_rrdot_data *>(P->opaque);

    /* This is pure geometry based on platform and target positions */
    /* Convert lat long to geocentric coordinates */
    PJ_XYZ xyz = Q->cart->fwd3d(lpz, Q->cart);

    /* Range vector */
    PJ_XYZ rvec = {(xyz.x - Q->pxyz.x) / P->a, (xyz.y - Q->pxyz.y) / P->a,
                   (xyz.z - Q->pxyz.z) / P->a};

    /* Check side - (Left && positive) || (Right && negative) */
    /* rvec.cross(vel).dot(pos) */
    double check = ((rvec.y * Q->vxyz.z - rvec.z * Q->vxyz.y) * Q->pxyz.x +
                    (rvec.z * Q->vxyz.x - rvec.x * Q->vxyz.z) * Q->pxyz.y +
                    (rvec.x * Q->vxyz.y - rvec.y * Q->vxyz.x) * Q->pxyz.z);

    if ((Q->side == -1) != (check > 0.)) {
        xyz.x = std::numeric_limits<double>::quiet_NaN();
        xyz.y = std::numeric_limits<double>::quiet_NaN();
        xyz.z = std::numeric_limits<double>::quiet_NaN();
        proj_errno_set(P, PROJ_ERR_COORD_TRANSFM_OUTSIDE_PROJECTION_DOMAIN);
        return xyz;
    }

    /* Slant range, Rdot, Height above Ellipse */
    xyz.x = sqrt(rvec.x * rvec.x + rvec.y * rvec.y + rvec.z * rvec.z);
    xyz.y = -(Q->vxyz.x * rvec.x + Q->vxyz.y * rvec.y + Q->vxyz.z * rvec.z) /
            (xyz.x * P->a);
    xyz.z = lpz.z;

    return xyz;
}

static PJ *pj_rrdot_destructor(PJ *P, int errlev) {
    if (nullptr == P)
        return nullptr;

    auto Q = static_cast<struct pj_rrdot_data *>(P->opaque);
    if (Q) {
        if (Q->cart)
            Q->cart->destructor(Q->cart, errlev);
    }

    return pj_default_destructor(P, errlev);
}

static PJ *pj_rrdot_setup(PJ *P) { /* general initialization */
    struct pj_rrdot_data *Q = static_cast<struct pj_rrdot_data *>(P->opaque);

    /* Setup original geocentric system */
    // Pass a dummy ellipsoid definition that will be overridden just afterwards
    Q->cart = proj_create(P->ctx, "+proj=cart +a=1");
    if (Q->cart == nullptr)
        return pj_rrdot_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);

    /* inherit ellipsoid definition from P to Q->cart */
    pj_inherit_ellipsoid_def(P, Q->cart);

    /* Get platform location for height */
    Q->dist = sqrt(Q->pxyz.x * Q->pxyz.x + Q->pxyz.y * Q->pxyz.y +
                   Q->pxyz.z * Q->pxyz.z);
    PJ_XYZ xyz = {Q->pxyz.x / P->a, Q->pxyz.y / P->a, Q->pxyz.z / P->b};
    double fact = 1.0 / sqrt(xyz.x * xyz.x + xyz.y * xyz.y + xyz.z * xyz.z);
    Q->radius = fact * Q->dist;
    Q->height = (1.0 - fact) * Q->dist;

    /* nhat is vector to center of ellipsoid */
    Q->nhat.x = -Q->pxyz.x / Q->dist;
    Q->nhat.y = -Q->pxyz.y / Q->dist;
    Q->nhat.z = -Q->pxyz.z / Q->dist;

    /* chat is orthogonal to velocity and geocentric vector */
    xyz.x = Q->nhat.y * Q->vxyz.z - Q->nhat.z * Q->vxyz.y;
    xyz.y = Q->nhat.z * Q->vxyz.x - Q->nhat.x * Q->vxyz.z;
    xyz.z = Q->nhat.x * Q->vxyz.y - Q->nhat.y * Q->vxyz.x;
    fact = sqrt(xyz.x * xyz.x + xyz.y * xyz.y + xyz.z * xyz.z);
    Q->chat.x = xyz.x / fact;
    Q->chat.y = xyz.y / fact;
    Q->chat.z = xyz.z / fact;

    /* that is orthogonal to nhat and chat*/
    xyz.x = Q->chat.y * Q->nhat.z - Q->chat.z * Q->nhat.y;
    xyz.y = Q->chat.z * Q->nhat.x - Q->chat.x * Q->nhat.z;
    xyz.z = Q->chat.x * Q->nhat.y - Q->chat.y * Q->nhat.x;
    fact = sqrt(xyz.x * xyz.x + xyz.y * xyz.y + xyz.z * xyz.z);
    Q->that.x = xyz.x / fact;
    Q->that.y = xyz.y / fact;
    Q->that.z = xyz.z / fact;

    /* Precompute dot factors */
    Q->vmag = sqrt(Q->vxyz.x * Q->vxyz.x + Q->vxyz.y * Q->vxyz.y +
                   Q->vxyz.z * Q->vxyz.z);
    Q->ndotv_vdott = (Q->nhat.x * Q->vxyz.x + Q->nhat.y * Q->vxyz.y +
                      Q->nhat.z * Q->vxyz.z) /
                     Q->vmag;
    Q->ndotv_vdott /= (Q->that.x * Q->vxyz.x + Q->that.y * Q->vxyz.y +
                       Q->that.z * Q->vxyz.z) /
                      Q->vmag;

    P->fwd3d = rrdot_forward3d;
    P->inv3d = rrdot_inverse3d;
    return P;
}

PJ *PJ_PROJECTION(rrdot) {
    struct pj_rrdot_data *Q = static_cast<struct pj_rrdot_data *>(
        calloc(1, sizeof(struct pj_rrdot_data)));
    if (nullptr == Q)
        return pj_default_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
    P->opaque = Q;
    P->destructor = pj_rrdot_destructor;

    /* Check if position was defined */
    if (pj_param(P->ctx, P->params, "tx").i)
        Q->pxyz.x = pj_param(P->ctx, P->params, "dx").f;
    else {
        proj_log_error(P, _("Missing parameter x."));
        return pj_default_destructor(P, PROJ_ERR_INVALID_OP_MISSING_ARG);
    }

    if (pj_param(P->ctx, P->params, "ty").i)
        Q->pxyz.y = pj_param(P->ctx, P->params, "dy").f;
    else {
        proj_log_error(P, _("Missing parameter y."));
        return pj_default_destructor(P, PROJ_ERR_INVALID_OP_MISSING_ARG);
    }

    if (pj_param(P->ctx, P->params, "tz").i)
        Q->pxyz.z = pj_param(P->ctx, P->params, "dz").f;
    else {
        proj_log_error(P, _("Missing parameter z."));
        return pj_default_destructor(P, PROJ_ERR_INVALID_OP_MISSING_ARG);
    }

    /* Check if velocity was defined */
    if (pj_param(P->ctx, P->params, "tvx").i)
        Q->vxyz.x = pj_param(P->ctx, P->params, "dvx").f;
    else {
        proj_log_error(P, _("Missing parameter vx."));
        return pj_default_destructor(P, PROJ_ERR_INVALID_OP_MISSING_ARG);
    }

    if (pj_param(P->ctx, P->params, "tvy").i)
        Q->vxyz.y = pj_param(P->ctx, P->params, "dvy").f;
    else {
        proj_log_error(P, _("Missing parameter vy."));
        return pj_default_destructor(P, PROJ_ERR_INVALID_OP_MISSING_ARG);
    }

    if (pj_param(P->ctx, P->params, "tvz").i)
        Q->vxyz.z = pj_param(P->ctx, P->params, "dvz").f;
    else {
        proj_log_error(P, _("Missing parameter vz."));
        return pj_default_destructor(P, PROJ_ERR_INVALID_OP_MISSING_ARG);
    }

    /* Check if left looking is defined */
    /* JPL / ISCE convention - Right side is -1 */
    Q->side = pj_param(P->ctx, P->params, "bleft").i ? 1 : -1;

    return pj_rrdot_setup(P);
}
