/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Perform overall coordinate system to coordinate system
 *           transformations (pj_transform() function) including reprojection
 *           and datum shifting.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2000, Frank Warmerdam
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

#include <math.h>
#include <string.h>

#include "projects.h"
#include "geocent.h"


/* Apply transformation to observation - in forward or inverse direction */
/* Copied from proj.h */
enum PJ_DIRECTION {
    PJ_FWD   =  1,   /* Forward    */
    PJ_IDENT =  0,   /* Do nothing */
    PJ_INV   = -1    /* Inverse    */
};
typedef enum PJ_DIRECTION PJ_DIRECTION;

/* Copied from proj.h FIXME */
int  proj_errno_reset (const PJ *P);


static int adjust_axis( projCtx ctx, const char *axis, int denormalize_flag,
                           long point_count, int point_offset,
                           double *x, double *y, double *z );

#ifndef SRS_WGS84_SEMIMAJOR
#define SRS_WGS84_SEMIMAJOR 6378137.0
#endif

#ifndef SRS_WGS84_ESQUARED
#define SRS_WGS84_ESQUARED 0.0066943799901413165
#endif

#define Dx_BF (defn->datum_params[0])
#define Dy_BF (defn->datum_params[1])
#define Dz_BF (defn->datum_params[2])
#define Rx_BF (defn->datum_params[3])
#define Ry_BF (defn->datum_params[4])
#define Rz_BF (defn->datum_params[5])
#define M_BF  (defn->datum_params[6])

/*
** This table is intended to indicate for any given error code in
** the range 0 to -56, whether that error will occur for all locations (ie.
** it is a problem with the coordinate system as a whole) in which case the
** value would be 0, or if the problem is with the point being transformed
** in which case the value is 1.
**
** At some point we might want to move this array in with the error message
** list or something, but while experimenting with it this should be fine.
**
**
** NOTE (2017-10-01): Non-transient errors really should have resulted in a
** PJ==0 during initialization, and hence should be handled at the level
** before calling pj_transform. The only obvious example of the contrary
** appears to be the PJD_ERR_GRID_AREA case, which may also be taken to
** mean "no grids available"
**
**
*/

static const int transient_error[60] = {
    /*             0  1  2  3  4  5  6  7  8  9   */
    /* 0 to 9 */   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 10 to 19 */ 0, 0, 0, 0, 1, 1, 0, 1, 1, 1,
    /* 20 to 29 */ 1, 0, 0, 0, 0, 0, 0, 1, 0, 0,
    /* 30 to 39 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 40 to 49 */ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
    /* 50 to 59 */ 1, 0, 1, 0, 1, 1, 1, 1, 0, 0 };



/* -------------------------------------------------------------------- */
/*      Transform unusual input coordinate axis orientation to          */
/*      standard form if needed.                                        */
/* -------------------------------------------------------------------- */
static int adjust_axes (PJ *P, PJ_DIRECTION dir, long n, int dist, double *x, double *y, double *z) {
    /* Nothing to do? */
    if (0==strcmp(P->axis,"enu"))
        return 0;

    return adjust_axis( P->ctx, P->axis,
                dir==PJ_FWD ? 1: 0, n, dist, x, y, z );
}



/* ----------------------------------------------------------------------- */
/*    Transform cartesian ("geocentric") source coordinates to lat/long,   */
/*    if needed                                                            */
/* ----------------------------------------------------------------------- */
static int geographic_to_cartesian (PJ *P, PJ_DIRECTION dir, long n, int dist, double *x, double *y, double *z) {
    int res;
    long i;
    double fac = P->to_meter;

    /* Nothing to do? */
    if (!P->is_geocent)
        return 0;

    if ( z == NULL ) {
        pj_ctx_set_errno( pj_get_ctx(P), PJD_ERR_GEOCENTRIC);
        return PJD_ERR_GEOCENTRIC;
    }

    if (PJ_FWD==dir) {
        fac = P->fr_meter;
        res = pj_geodetic_to_geocentric( P->a_orig, P->es_orig, n, dist, x, y, z );
        if (res)
            return res;
    }

    if (fac != 1.0) {
        for( i = 0; i < n; i++ ) {
            if( x[dist*i] != HUGE_VAL ) {
                x[dist*i] *= fac;
                y[dist*i] *= fac;
                z[dist*i] *= fac;
            }
        }
    }

    if (PJ_FWD==dir)
        return 0;
    return pj_geocentric_to_geodetic(
        P->a_orig, P->es_orig,
        n, dist,
        x, y, z
    );
}










/* -------------------------------------------------------------------- */
/*      Transform destination points to projection coordinates, if      */
/*      desired.                                                        */
/*                                                                      */
/*      Ought to fold this into projected_to_geographic                 */
/* -------------------------------------------------------------------- */
static int geographic_to_projected (PJ *P, long n, int dist, double *x, double *y, double *z) {
    long i;

    /* Nothing to do? */
    if (P->is_latlong)
        return 0;
    if (P->is_geocent)
        return 0;

    if(P->fwd3d != NULL)
    {
        /* Three dimensions must be defined */
        if ( z == NULL)
        {
            pj_ctx_set_errno( pj_get_ctx(P), PJD_ERR_GEOCENTRIC);
            return PJD_ERR_GEOCENTRIC;
        }

        for( i = 0; i < n; i++ )
        {
            XYZ projected_loc;
            LPZ geodetic_loc;

            geodetic_loc.u = x[dist*i];
            geodetic_loc.v = y[dist*i];
            geodetic_loc.w = z[dist*i];

            if (geodetic_loc.u == HUGE_VAL)
                continue;

            proj_errno_reset( P );
            projected_loc = pj_fwd3d( geodetic_loc, P);
            if( P->ctx->last_errno != 0 )
            {
                if( (P->ctx->last_errno != 33 /*EDOM*/
                        && P->ctx->last_errno != 34 /*ERANGE*/ )
                    && (P->ctx->last_errno > 0
                        || P->ctx->last_errno < -44 || n == 1
                        || transient_error[-P->ctx->last_errno] == 0 ) )
                    return P->ctx->last_errno;
                else
                {
                    projected_loc.u = HUGE_VAL;
                    projected_loc.v = HUGE_VAL;
                    projected_loc.w = HUGE_VAL;
                }
            }

            x[dist*i] = projected_loc.u;
            y[dist*i] = projected_loc.v;
            z[dist*i] = projected_loc.w;
        }
        return 0;
    }

    for( i = 0; i <n; i++ )
    {
        XY         projected_loc;
        LP	       geodetic_loc;

        geodetic_loc.u = x[dist*i];
        geodetic_loc.v = y[dist*i];

        if( geodetic_loc.u == HUGE_VAL )
            continue;

        proj_errno_reset( P );
        projected_loc = pj_fwd( geodetic_loc, P );
        if( P->ctx->last_errno != 0 )
        {
            if( (P->ctx->last_errno != 33 /*EDOM*/
                    && P->ctx->last_errno != 34 /*ERANGE*/ )
                && (P->ctx->last_errno > 0
                    || P->ctx->last_errno < -44 || n == 1
                    || transient_error[-P->ctx->last_errno] == 0 ) )
                return P->ctx->last_errno;
            else
            {
                projected_loc.u = HUGE_VAL;
                projected_loc.v = HUGE_VAL;
            }
        }

        x[dist*i] = projected_loc.u;
        y[dist*i] = projected_loc.v;
    }
    return 0;
}





/* ----------------------------------------------------------------------- */
/*    Transform projected source coordinates to lat/long, if needed        */
/* ----------------------------------------------------------------------- */
static int projected_to_geographic (PJ *P, long n, int dist, double *x, double *y, double *z) {
    long i;

    /* Nothing to do? */
    if (P->is_latlong)
        return 0;

    /* Check first if projection is invertible. */
    if( (P->inv3d == NULL) && (P->inv == NULL))
    {
        pj_ctx_set_errno( pj_get_ctx(P), -17 );
        pj_log( pj_get_ctx(P), PJ_LOG_ERROR,
                "pj_transform(): source projection not invertable" );
        return -17;
    }

    /* If invertible - First try inv3d if defined */
    if (P->inv3d != NULL)
    {
        /* Three dimensions must be defined */
        if ( z == NULL)
        {
            pj_ctx_set_errno( pj_get_ctx(P), PJD_ERR_GEOCENTRIC);
            return PJD_ERR_GEOCENTRIC;
        }

        for (i=0; i < n; i++)
        {
            XYZ projected_loc;
            XYZ geodetic_loc;

            projected_loc.u = x[dist*i];
            projected_loc.v = y[dist*i];
            projected_loc.w = z[dist*i];

            if (projected_loc.u == HUGE_VAL)
                continue;

            proj_errno_reset( P );
            geodetic_loc = pj_inv3d(projected_loc, P);
            if( P->ctx->last_errno != 0 )
            {
                if( (P->ctx->last_errno != 33 /*EDOM*/
                        && P->ctx->last_errno != 34 /*ERANGE*/ )
                    && (P->ctx->last_errno > 0
                        || P->ctx->last_errno < -44 || n == 1
                        || transient_error[-P->ctx->last_errno] == 0 ) )
                    return P->ctx->last_errno;
                else
                {
                    geodetic_loc.u = HUGE_VAL;
                    geodetic_loc.v = HUGE_VAL;
                    geodetic_loc.w = HUGE_VAL;
                }
            }

            x[dist*i] = geodetic_loc.u;
            y[dist*i] = geodetic_loc.v;
            z[dist*i] = geodetic_loc.w;

        }
        return 0;
    }

    /* Fallback to the original PROJ.4 API 2d inversion - inv */
    for( i = 0; i < n; i++ ) {
        XY         projected_loc;
        LP	       geodetic_loc;

        projected_loc.u = x[dist*i];
        projected_loc.v = y[dist*i];

        if( projected_loc.u == HUGE_VAL )
            continue;

        proj_errno_reset( P );
        geodetic_loc = pj_inv( projected_loc, P );
        if( P->ctx->last_errno != 0 )
        {
            if( (P->ctx->last_errno != 33 /*EDOM*/
                    && P->ctx->last_errno != 34 /*ERANGE*/ )
                && (P->ctx->last_errno > 0
                    || P->ctx->last_errno < -44 || n == 1
                    || transient_error[-P->ctx->last_errno] == 0 ) )
                return P->ctx->last_errno;
            else
            {
                geodetic_loc.u = HUGE_VAL;
                geodetic_loc.v = HUGE_VAL;
            }
        }

        x[dist*i] = geodetic_loc.u;
        y[dist*i] = geodetic_loc.v;
    }
    return 0;
}



/* -------------------------------------------------------------------- */
/*            Adjust for the prime meridian if needed.                  */
/* -------------------------------------------------------------------- */
static int prime_meridian (PJ *P, PJ_DIRECTION dir, long n, int dist, double *x) {
    int i;
    double pm = P->from_greenwich;

    /* Nothing to do? */
    if (pm==0.0)
        return 0;
    if (!(P->is_geocent || P->is_latlong))
        return 0;

    if (dir==PJ_FWD)
        pm = -pm;

    for (i = 0;  i < n;  i++)
        if (x[dist*i] != HUGE_VAL)
            x[dist*i] += pm;

    return 0;
}



/* -------------------------------------------------------------------- */
/*            Adjust for vertical scale factor if needed                */
/* -------------------------------------------------------------------- */
static int height_unit (PJ *P, PJ_DIRECTION dir, long n, int dist, double *z) {
    int i;
    double fac = P->vto_meter;

    if (PJ_FWD==dir)
        fac = P->vfr_meter;

    /* Nothing to do? */
    if (fac==1.0)
        return 0;
    if (0==z)
        return 0;

    for (i = 0;  i < n;  i++)
        if (z[dist*i] != HUGE_VAL )
            z[dist*i] *= fac;

    return 0;
}



/* -------------------------------------------------------------------- */
/*           Transform to ellipsoidal heights if needed                 */
/* -------------------------------------------------------------------- */
static int geometric_to_orthometric (PJ *P, PJ_DIRECTION dir, long n, int dist, double *x, double *y, double *z) {
    int err;
    if (0==P->has_geoid_vgrids)
        return 0;
    if (z==0)
        return PJD_ERR_GEOCENTRIC;
    err = pj_apply_vgridshift (P, "sgeoidgrids",
              &(P->vgridlist_geoid),
              &(P->vgridlist_geoid_count),
              dir==PJ_FWD ? 1 : 0, n, dist, x, y, z );
    if (err)
        return pj_ctx_get_errno(P->ctx);
    return 0;
}



/* -------------------------------------------------------------------- */
/*      Convert datums if needed, and possible.                         */
/* -------------------------------------------------------------------- */
static int datum_transform (PJ *P, PJ *Q, long n, int dist, double *x, double *y, double *z) {
    if (0==pj_datum_transform (P, Q, n, dist, x, y, z))
        return 0;
    if (P->ctx->last_errno)
        return P->ctx->last_errno;
    return Q->ctx->last_errno;
}





/* -------------------------------------------------------------------- */
/*      If a wrapping center other than 0 is provided, rewrap around    */
/*      the suggested center (for latlong coordinate systems only).     */
/* -------------------------------------------------------------------- */
static int long_wrap (PJ *P, long n, int dist, double *x) {
    long i;

    /* Nothing to do? */
    if (P->is_geocent)
        return 0;
    if (!P->is_long_wrap_set)
        return 0;
    if (!P->is_latlong)
        return 0;

    for (i = 0;  i < n;  i++ ) {
        double val = x[dist*i];
        if (val == HUGE_VAL)
            continue;

        /* Get fast in ] -2 PI, 2 PI [ range */
        val = fmod(val, M_TWOPI);
        while( val < P->long_wrap_center - M_PI )
            val += M_TWOPI;
        while( val > P->long_wrap_center + M_PI )
            val -= M_TWOPI;
        x[dist*i] = val;
    }
    return 0;
}



/************************************************************************/
/*                            pj_transform()                            */
/*                                                                      */
/*      Currently this function doesn't recognise if two projections    */
/*      are identical (to short circuit reprojection) because it is     */
/*      difficult to compare PJ structures (since there are some        */
/*      projection specific components).                                */
/************************************************************************/

int pj_transform(
    PJ *src, PJ *dst,
    long point_count, int point_offset,
    double *x, double *y, double *z
){
    int       err;

    src->ctx->last_errno = 0;
    dst->ctx->last_errno = 0;

    if( point_offset == 0 )
        point_offset = 1;

    /* Bring input to "normal form": longitude, latitude, ellipsoidal height */

    err = adjust_axes (src, PJ_INV, point_count, point_offset, x, y, z);
    if (err)
        return err;
    err = geographic_to_cartesian (src, PJ_INV, point_count, point_offset, x, y, z);
    if (err)
        return err;
    err = projected_to_geographic (src, point_count, point_offset, x, y, z);
    if (err)
        return err;
    err = prime_meridian (src, PJ_INV, point_count, point_offset, x);
    if (err)
        return err;
    err = height_unit (src, PJ_INV, point_count, point_offset, z);
    if (err)
        return err;
    err = geometric_to_orthometric (src, PJ_INV, point_count, point_offset, x, y, z);
    if (err)
        return err;

    /* At the center of the process we do the datum shift (if needed) */

    err = datum_transform(src, dst, point_count, point_offset, x, y, z );
    if (err)
        return err;

    /* Now get out on the other side: Bring "normal form" to output form */

    err = geometric_to_orthometric (dst, PJ_FWD, point_count, point_offset, x, y, z);
    if (err)
        return err;
    err = height_unit (dst, PJ_FWD, point_count, point_offset, z);
    if (err)
        return err;
    err = prime_meridian (dst, PJ_FWD, point_count, point_offset, x);
    if (err)
        return err;
    err = geographic_to_cartesian (dst, PJ_FWD, point_count, point_offset, x, y, z);
    if (err)
        return err;
    err = geographic_to_projected (dst, point_count, point_offset, x, y, z);
    if (err)
        return err;
    err = long_wrap (dst, point_count, point_offset, x);
    if (err)
        return err;
    err = adjust_axes (dst, PJ_FWD, point_count, point_offset, x, y, z);
    if (err)
        return err;

    return 0;
}



/************************************************************************/
/*                     pj_geodetic_to_geocentric()                      */
/************************************************************************/

int pj_geodetic_to_geocentric( double a, double es,
                               long point_count, int point_offset,
                               double *x, double *y, double *z )

{
    double b;
    int    i;
    GeocentricInfo gi;
    int ret_errno = 0;

    if( es == 0.0 )
        b = a;
    else
        b = a * sqrt(1-es);

    if( pj_Set_Geocentric_Parameters( &gi, a, b ) != 0 )
    {
        return PJD_ERR_GEOCENTRIC;
    }

    for( i = 0; i < point_count; i++ )
    {
        long io = i * point_offset;

        if( x[io] == HUGE_VAL  )
            continue;

        if( pj_Convert_Geodetic_To_Geocentric( &gi, y[io], x[io], z[io],
                                               x+io, y+io, z+io ) != 0 )
        {
            ret_errno = PJD_ERR_LAT_OR_LON_EXCEED_LIMIT;
            x[io] = y[io] = HUGE_VAL;
            /* but keep processing points! */
        }
    }

    return ret_errno;
}

/************************************************************************/
/*                     pj_geodetic_to_geocentric()                      */
/************************************************************************/

int pj_geocentric_to_geodetic( double a, double es,
                               long point_count, int point_offset,
                               double *x, double *y, double *z )

{
    double b;
    int    i;
    GeocentricInfo gi;

    if( es == 0.0 )
        b = a;
    else
        b = a * sqrt(1-es);

    if( pj_Set_Geocentric_Parameters( &gi, a, b ) != 0 )
    {
        return PJD_ERR_GEOCENTRIC;
    }

    for( i = 0; i < point_count; i++ )
    {
        long io = i * point_offset;

        if( x[io] == HUGE_VAL )
            continue;

        pj_Convert_Geocentric_To_Geodetic( &gi, x[io], y[io], z[io],
                                           y+io, x+io, z+io );
    }

    return 0;
}

/************************************************************************/
/*                         pj_compare_datums()                          */
/*                                                                      */
/*      Returns TRUE if the two datums are identical, otherwise         */
/*      FALSE.                                                          */
/************************************************************************/

int pj_compare_datums( PJ *srcdefn, PJ *dstdefn )

{
    if( srcdefn->datum_type != dstdefn->datum_type )
    {
        return 0;
    }
    else if( srcdefn->a_orig != dstdefn->a_orig
             || ABS(srcdefn->es_orig - dstdefn->es_orig) > 0.000000000050 )
    {
        /* the tolerance for es is to ensure that GRS80 and WGS84 are
           considered identical */
        return 0;
    }
    else if( srcdefn->datum_type == PJD_3PARAM )
    {
        return (srcdefn->datum_params[0] == dstdefn->datum_params[0]
                && srcdefn->datum_params[1] == dstdefn->datum_params[1]
                && srcdefn->datum_params[2] == dstdefn->datum_params[2]);
    }
    else if( srcdefn->datum_type == PJD_7PARAM )
    {
        return (srcdefn->datum_params[0] == dstdefn->datum_params[0]
                && srcdefn->datum_params[1] == dstdefn->datum_params[1]
                && srcdefn->datum_params[2] == dstdefn->datum_params[2]
                && srcdefn->datum_params[3] == dstdefn->datum_params[3]
                && srcdefn->datum_params[4] == dstdefn->datum_params[4]
                && srcdefn->datum_params[5] == dstdefn->datum_params[5]
                && srcdefn->datum_params[6] == dstdefn->datum_params[6]);
    }
    else if( srcdefn->datum_type == PJD_GRIDSHIFT )
    {
        const char* srcnadgrids =
            pj_param(srcdefn->ctx, srcdefn->params,"snadgrids").s;
        const char* dstnadgrids =
            pj_param(dstdefn->ctx, dstdefn->params,"snadgrids").s;
        return srcnadgrids != 0 && dstnadgrids != 0 &&
               strcmp( srcnadgrids, dstnadgrids ) == 0;
    }
    else
        return 1;
}

/************************************************************************/
/*                       pj_geocentic_to_wgs84()                        */
/************************************************************************/

static
int pj_geocentric_to_wgs84( PJ *defn,
                            long point_count, int point_offset,
                            double *x, double *y, double *z )

{
    int       i;

    if( defn->datum_type == PJD_3PARAM )
    {
        for( i = 0; i < point_count; i++ )
        {
            long io = i * point_offset;

            if( x[io] == HUGE_VAL )
                continue;

            x[io] = x[io] + Dx_BF;
            y[io] = y[io] + Dy_BF;
            z[io] = z[io] + Dz_BF;
        }
    }
    else if( defn->datum_type == PJD_7PARAM )
    {
        for( i = 0; i < point_count; i++ )
        {
            long io = i * point_offset;
            double x_out, y_out, z_out;

            if( x[io] == HUGE_VAL )
                continue;

            x_out = M_BF*(       x[io] - Rz_BF*y[io] + Ry_BF*z[io]) + Dx_BF;
            y_out = M_BF*( Rz_BF*x[io] +       y[io] - Rx_BF*z[io]) + Dy_BF;
            z_out = M_BF*(-Ry_BF*x[io] + Rx_BF*y[io] +       z[io]) + Dz_BF;

            x[io] = x_out;
            y[io] = y_out;
            z[io] = z_out;
        }
    }

    return 0;
}

/************************************************************************/
/*                      pj_geocentic_from_wgs84()                       */
/************************************************************************/

static
int pj_geocentric_from_wgs84( PJ *defn,
                              long point_count, int point_offset,
                              double *x, double *y, double *z )

{
    int       i;

    if( defn->datum_type == PJD_3PARAM )
    {
        for( i = 0; i < point_count; i++ )
        {
            long io = i * point_offset;

            if( x[io] == HUGE_VAL )
                continue;

            x[io] = x[io] - Dx_BF;
            y[io] = y[io] - Dy_BF;
            z[io] = z[io] - Dz_BF;
        }
    }
    else if( defn->datum_type == PJD_7PARAM )
    {
        for( i = 0; i < point_count; i++ )
        {
            long io = i * point_offset;
            double x_tmp, y_tmp, z_tmp;

            if( x[io] == HUGE_VAL )
                continue;

            x_tmp = (x[io] - Dx_BF) / M_BF;
            y_tmp = (y[io] - Dy_BF) / M_BF;
            z_tmp = (z[io] - Dz_BF) / M_BF;

            x[io] =        x_tmp + Rz_BF*y_tmp - Ry_BF*z_tmp;
            y[io] = -Rz_BF*x_tmp +       y_tmp + Rx_BF*z_tmp;
            z[io] =  Ry_BF*x_tmp - Rx_BF*y_tmp +       z_tmp;
        }
    }

    return 0;
}

/************************************************************************/
/*                         pj_datum_transform()                         */
/*                                                                      */
/*      The input should be long/lat/z coordinates in radians in the    */
/*      source datum, and the output should be long/lat/z               */
/*      coordinates in radians in the destination datum.                */
/************************************************************************/

int pj_datum_transform( PJ *src, PJ *dst,
                        long point_count, int point_offset,
                        double *x, double *y, double *z )

{
    double      src_a, src_es, dst_a, dst_es;
    int         z_is_temp = FALSE;

/* -------------------------------------------------------------------- */
/*      We cannot do any meaningful datum transformation if either      */
/*      the source or destination are of an unknown datum type          */
/*      (ie. only a +ellps declaration, no +datum).  This is new        */
/*      behavior for PROJ 4.6.0.                                        */
/* -------------------------------------------------------------------- */
    if( src->datum_type == PJD_UNKNOWN
        || dst->datum_type == PJD_UNKNOWN )
        return 0;

/* -------------------------------------------------------------------- */
/*      Short cut if the datums are identical.                          */
/* -------------------------------------------------------------------- */
    if( pj_compare_datums( src, dst ) )
        return 0;

    src_a = src->a_orig;
    src_es = src->es_orig;

    dst_a = dst->a_orig;
    dst_es = dst->es_orig;

/* -------------------------------------------------------------------- */
/*      Create a temporary Z array if one is not provided.              */
/* -------------------------------------------------------------------- */
    if( z == NULL )
    {
        int	bytes = sizeof(double) * point_count * point_offset;
        z = (double *) pj_malloc(bytes);
        memset( z, 0, bytes );
        z_is_temp = TRUE;
    }

#define CHECK_RETURN(defn) {if( defn->ctx->last_errno != 0 && (defn->ctx->last_errno > 0 || transient_error[-defn->ctx->last_errno] == 0) ) { if( z_is_temp ) pj_dalloc(z); return defn->ctx->last_errno; }}

/* -------------------------------------------------------------------- */
/*	If this datum requires grid shifts, then apply it to geodetic   */
/*      coordinates.                                                    */
/* -------------------------------------------------------------------- */
    if( src->datum_type == PJD_GRIDSHIFT )
    {
        pj_apply_gridshift_2( src, 0, point_count, point_offset, x, y, z );
        CHECK_RETURN(src);

        src_a = SRS_WGS84_SEMIMAJOR;
        src_es = SRS_WGS84_ESQUARED;
    }

    if( dst->datum_type == PJD_GRIDSHIFT )
    {
        dst_a = SRS_WGS84_SEMIMAJOR;
        dst_es = SRS_WGS84_ESQUARED;
    }

/* ==================================================================== */
/*      Do we need to go through geocentric coordinates?                */
/* ==================================================================== */
    if( src_es != dst_es || src_a != dst_a
        || src->datum_type == PJD_3PARAM
        || src->datum_type == PJD_7PARAM
        || dst->datum_type == PJD_3PARAM
        || dst->datum_type == PJD_7PARAM)
    {
/* -------------------------------------------------------------------- */
/*      Convert to geocentric coordinates.                              */
/* -------------------------------------------------------------------- */
        src->ctx->last_errno =
            pj_geodetic_to_geocentric( src_a, src_es,
                                       point_count, point_offset, x, y, z );
        CHECK_RETURN(src);

/* -------------------------------------------------------------------- */
/*      Convert between datums.                                         */
/* -------------------------------------------------------------------- */
        if( src->datum_type == PJD_3PARAM
            || src->datum_type == PJD_7PARAM )
        {
            pj_geocentric_to_wgs84( src, point_count, point_offset,x,y,z);
            CHECK_RETURN(src);
        }

        if( dst->datum_type == PJD_3PARAM
            || dst->datum_type == PJD_7PARAM )
        {
            pj_geocentric_from_wgs84( dst, point_count,point_offset,x,y,z);
            CHECK_RETURN(dst);
        }

/* -------------------------------------------------------------------- */
/*      Convert back to geodetic coordinates.                           */
/* -------------------------------------------------------------------- */
        dst->ctx->last_errno =
            pj_geocentric_to_geodetic( dst_a, dst_es,
                                       point_count, point_offset, x, y, z );
        CHECK_RETURN(dst);
    }

/* -------------------------------------------------------------------- */
/*      Apply grid shift to destination if required.                    */
/* -------------------------------------------------------------------- */
    if( dst->datum_type == PJD_GRIDSHIFT )
    {
        pj_apply_gridshift_2( dst, 1, point_count, point_offset, x, y, z );
        CHECK_RETURN(dst);
    }

    if( z_is_temp )
        pj_dalloc( z );

    return 0;
}

/************************************************************************/
/*                           adjust_axis()                           */
/*                                                                      */
/*      Normalize or de-normalized the x/y/z axes.  The normal form     */
/*      is "enu" (easting, northing, up).                               */
/************************************************************************/
static int adjust_axis( projCtx ctx,
                           const char *axis, int denormalize_flag,
                           long point_count, int point_offset,
                           double *x, double *y, double *z )

{
    double x_in, y_in, z_in = 0.0;
    int i, i_axis;

    if( !denormalize_flag )
    {
        for( i = 0; i < point_count; i++ )
        {
            x_in = x[point_offset*i];
            y_in = y[point_offset*i];
            if( z )
                z_in = z[point_offset*i];

            for( i_axis = 0; i_axis < 3; i_axis++ )
            {
                double value;

                if( i_axis == 0 )
                    value = x_in;
                else if( i_axis == 1 )
                    value = y_in;
                else
                    value = z_in;

                switch( axis[i_axis] )
                {
                  case 'e':
                    x[point_offset*i] = value;
                    break;
                  case 'w':
                    x[point_offset*i] = -value;
                    break;
                  case 'n':
                    y[point_offset*i] = value;
                    break;
                  case 's':
                    y[point_offset*i] = -value;
                    break;
                  case 'u':
                    if( z )
                        z[point_offset*i] = value;
                    break;
                  case 'd':
                    if( z )
                        z[point_offset*i] = -value;
                    break;
                  default:
                    pj_ctx_set_errno( ctx, PJD_ERR_AXIS );
                    return PJD_ERR_AXIS;
                }
            } /* i_axis */
        } /* i (point) */
    }

    else /* denormalize */
    {
        for( i = 0; i < point_count; i++ )
        {
            x_in = x[point_offset*i];
            y_in = y[point_offset*i];
            if( z )
                z_in = z[point_offset*i];

            for( i_axis = 0; i_axis < 3; i_axis++ )
            {
                double *target;

                if( i_axis == 2 && z == NULL )
                    continue;

                if( i_axis == 0 )
                    target = x;
                else if( i_axis == 1 )
                    target = y;
                else
                    target = z;

                switch( axis[i_axis] )
                {
                  case 'e':
                    target[point_offset*i] = x_in; break;
                  case 'w':
                    target[point_offset*i] = -x_in; break;
                  case 'n':
                    target[point_offset*i] = y_in; break;
                  case 's':
                    target[point_offset*i] = -y_in; break;
                  case 'u':
                    target[point_offset*i] = z_in; break;
                  case 'd':
                    target[point_offset*i] = -z_in; break;
                  default:
                    pj_ctx_set_errno( ctx, PJD_ERR_AXIS );
                    return PJD_ERR_AXIS;
                }
            } /* i_axis */
        } /* i (point) */
    }

    return 0;
}
