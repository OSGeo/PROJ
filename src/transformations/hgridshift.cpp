#define PJ_LIB__

#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <time.h>

#include "proj_internal.h"
#include "grids.hpp"

PROJ_HEAD(hgridshift, "Horizontal grid shift");

using namespace NS_PROJ;

#ifdef __MINGW32__
// mingw32-win32 doesn't implement std::mutex
namespace {
class MyMutex {
  public:
    // cppcheck-suppress functionStatic
    void lock() { pj_acquire_lock(); }
    // cppcheck-suppress functionStatic
    void unlock() { pj_release_lock(); }
};
}
#else
#include <mutex>
#define MyMutex std::mutex
#endif

static MyMutex gMutex{};
static std::set<std::string> gKnownGrids{};

namespace { // anonymous namespace
struct hgridshiftData {
    double t_final = 0;
    double t_epoch = 0;
    ListOfHGrids grids{};
    bool defer_grid_opening = false;
};
} // anonymous namespace

static PJ_XYZ forward_3d(PJ_LPZ lpz, PJ *P) {
    auto Q = static_cast<hgridshiftData*>(P->opaque);
    PJ_COORD point = {{0,0,0,0}};
    point.lpz = lpz;

    if ( Q->defer_grid_opening ) {
        Q->defer_grid_opening = false;
        Q->grids = pj_hgrid_init(P, "grids");
        if ( proj_errno(P) ) {
            return proj_coord_error().xyz;
        }
    }

    if (!Q->grids.empty()) {
        /* Only try the gridshift if at least one grid is loaded,
         * otherwise just pass the coordinate through unchanged. */
        point.lp = pj_hgrid_apply(P->ctx, Q->grids, point.lp, PJ_FWD);
    }

    return point.xyz;
}


static PJ_LPZ reverse_3d(PJ_XYZ xyz, PJ *P) {
    auto Q = static_cast<hgridshiftData*>(P->opaque);
    PJ_COORD point = {{0,0,0,0}};
    point.xyz = xyz;

    if ( Q->defer_grid_opening ) {
        Q->defer_grid_opening = false;
        Q->grids = pj_hgrid_init(P, "grids");
        if ( proj_errno(P) ) {
            return proj_coord_error().lpz;
        }
    }

    if (!Q->grids.empty()) {
        /* Only try the gridshift if at least one grid is loaded,
         * otherwise just pass the coordinate through unchanged. */
        point.lp = pj_hgrid_apply(P->ctx, Q->grids, point.lp, PJ_INV);
    }

    return point.lpz;
}

static PJ_COORD forward_4d(PJ_COORD obs, PJ *P) {
    struct hgridshiftData *Q = (struct hgridshiftData *) P->opaque;
    PJ_COORD point = obs;

    /* If transformation is not time restricted, we always call it */
    if (Q->t_final==0 || Q->t_epoch==0) {
        point.xyz = forward_3d (obs.lpz, P);
        return point;
    }

    /* Time restricted - only apply transform if within time bracket */
    if (obs.lpzt.t < Q->t_epoch && Q->t_final > Q->t_epoch)
        point.xyz = forward_3d (obs.lpz, P);


    return point;
}

static PJ_COORD reverse_4d(PJ_COORD obs, PJ *P) {
    struct hgridshiftData *Q = (struct hgridshiftData *) P->opaque;
    PJ_COORD point = obs;

    /* If transformation is not time restricted, we always call it */
    if (Q->t_final==0 || Q->t_epoch==0) {
        point.lpz = reverse_3d (obs.xyz, P);
        return point;
    }

    /* Time restricted - only apply transform if within time bracket */
    if (obs.lpzt.t < Q->t_epoch && Q->t_final > Q->t_epoch)
        point.lpz = reverse_3d (obs.xyz, P);

    return point;
}

static PJ *destructor (PJ *P, int errlev) {
    if (nullptr==P)
        return nullptr;

    delete static_cast<struct hgridshiftData*>(P->opaque);
    P->opaque = nullptr;

    return pj_default_destructor(P, errlev);
}

static void reassign_context( PJ* P, PJ_CONTEXT* ctx )
{
    auto Q = (struct hgridshiftData *) P->opaque;
    for( auto& grid: Q->grids ) {
        grid->reassign_context(ctx);
    }
}

PJ *TRANSFORMATION(hgridshift,0) {
    auto Q = new hgridshiftData;
    P->opaque = (void *) Q;
    P->destructor = destructor;
    P->reassign_context = reassign_context;

    P->fwd4d  = forward_4d;
    P->inv4d  = reverse_4d;
    P->fwd3d  = forward_3d;
    P->inv3d  = reverse_3d;
    P->fwd    = nullptr;
    P->inv    = nullptr;

    P->left  = PJ_IO_UNITS_RADIANS;
    P->right = PJ_IO_UNITS_RADIANS;

    if (0==pj_param(P->ctx, P->params, "tgrids").i) {
        proj_log_error(P, "hgridshift: +grids parameter missing.");
        return destructor (P, PJD_ERR_NO_ARGS);
    }

    /* TODO: Refactor into shared function that can be used  */
    /*       by both vgridshift and hgridshift               */
    if (pj_param(P->ctx, P->params, "tt_final").i) {
        Q->t_final = pj_param (P->ctx, P->params, "dt_final").f;
        if (Q->t_final == 0) {
            /* a number wasn't passed to +t_final, let's see if it was "now" */
            /* and set the time accordingly.                                 */
            if (!strcmp("now", pj_param(P->ctx, P->params, "st_final").s)) {
                    time_t now;
                    struct tm *date;
                    time(&now);
                    date = localtime(&now);
                    Q->t_final = 1900.0 + date->tm_year + date->tm_yday/365.0;
            }
        }
    }

    if (pj_param(P->ctx, P->params, "tt_epoch").i)
        Q->t_epoch = pj_param (P->ctx, P->params, "dt_epoch").f;

    if( P->ctx->defer_grid_opening ) {
        Q->defer_grid_opening = true;
    }
    else {
        const char *gridnames = pj_param(P->ctx, P->params, "sgrids").s;
        gMutex.lock();
        const bool isKnownGrid = gKnownGrids.find(gridnames) != gKnownGrids.end();
        gMutex.unlock();
        if( isKnownGrid ) {
            Q->defer_grid_opening = true;
        }
        else {
            Q->grids = pj_hgrid_init(P, "grids");
            /* Was gridlist compiled properly? */
            if ( proj_errno(P) ) {
                proj_log_error(P, "hgridshift: could not find required grid(s).");
                return destructor(P, PJD_ERR_FAILED_TO_LOAD_GRID);
            }

            gMutex.lock();
            gKnownGrids.insert(gridnames);
            gMutex.unlock();
        }
    }

    return P;
}

void pj_clear_hgridshift_knowngrids_cache() {
    gMutex.lock();
    gKnownGrids.clear();
    gMutex.unlock();
}
