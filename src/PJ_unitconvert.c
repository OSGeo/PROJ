/***********************************************************************

            Unit conversion pseudo-projection for use with
                       transformation pipelines.

                      Kristian Evers, 2017-05-16

************************************************************************

A pseudo-projection that can be used to convert units of input and
output data. Primarily useful in pipelines.

Unit conversion is performed by means of a pivot unit. The pivot unit
for distance units are the meter and for time we use the modified julian
date. A time unit conversion is performed like

    Unit A -> Modified Julian date -> Unit B

distance units are converted in the same manner, with meter being the
central unit.

The modified Julian date is chosen as the pivout unit since it has a
fairly high precision, goes sufficiently long backwards in time, has no
danger of hitting the upper limit in the near future and it is a fairly
common time unit in astronomy and geodesy. Note that we are using the
Julian date and not day. The difference being that the latter is defined
as an integer and is thus limited to days in resolution. This approach
has been extended wherever it makes sense, e.g. the GPS week unit also
has a fractional part that makes it possible to determine the day, hour
and minute of an observation.

In- and output units are controlled with the parameters

    +xy_in, +xy_out, +z_in, +z_out, +t_in and +t_out

where xy denotes horizontal units, z vertical units and t time units.

************************************************************************

Kristian Evers, kreve@sdfe.dk, 2017-05-09
Last update: 2017-05-16

************************************************************************
* Copyright (c) 2017, Kristian Evers / SDFE
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
*
***********************************************************************/

#define PJ_LIB__
#include <time.h>
#include "proj_internal.h"
#include <projects.h>
#include <errno.h>

PROJ_HEAD(unitconvert, "Unit conversion");

typedef double (*tconvert)(double);

struct TIME_UNITS {
    char        *id;        /* units keyword */
    tconvert     t_in;      /* unit -> mod. julian date funtion pointer */
    tconvert     t_out;     /* mod. julian date > unit function pointer */
    char        *name;      /* comments */
};


/***********************************************************************/
static int is_leap_year(int year) {
/***********************************************************************/
    return ((year % 4 == 0 && year % 100 != 0) || year % 400 ==0);
}


/***********************************************************************/
static int days_in_year(int year) {
/***********************************************************************/
    return is_leap_year(year) ? 366 : 365;
}


/***********************************************************************/
static double mjd_to_mjd(double mjd) {
/***********************************************************************
    Modified julian date no-op function.

    The Julian date is defined as (fractional) days since midnight
    on 16th of November in 1858.
************************************************************************/
    return mjd;
}


/***********************************************************************/
static double decimalyear_to_mjd(double decimalyear) {
/***********************************************************************
    Epoch of modified julian date is 1858-11-16 00:00
************************************************************************/
    int year = (int)floor(decimalyear);
    double fractional_year = decimalyear - year;
    double mjd = (year - 1859)*365 + 14 + 31;
    mjd += fractional_year*days_in_year(year);

    /* take care of leap days */
    year--;
    for (; year > 1858; year--)
        if (is_leap_year(year))
            mjd++;

    return mjd;
}


/***********************************************************************/
static double mjd_to_decimalyear(double mjd) {
/***********************************************************************
    Epoch of modified julian date is 1858-11-16 00:00
************************************************************************/
    double decimalyear = mjd;
    double mjd_iter = 14 + 31;
    int year = 1859;

    /* a smarter brain than mine could probably to do this more elegantly
       - I'll just brute-force my way out of this... */
    for (; mjd >= mjd_iter; year++) {
        mjd_iter += days_in_year(year);
    }
    year--;
    mjd_iter -= days_in_year(year);

    decimalyear = year + (mjd-mjd_iter)/days_in_year(year);
    return decimalyear;
}


/***********************************************************************/
static double gps_week_to_mjd(double gps_week) {
/***********************************************************************
    GPS weeks are defined as the number of weeks since January the 6th
    1980.

    Epoch of gps weeks is 1980-01-06 00:00, which in modified Julian
    date is 44244.
************************************************************************/
    return 44244.0 + gps_week*7.0;
}


/***********************************************************************/
static double mjd_to_gps_week(double mjd) {
/***********************************************************************
    GPS weeks are defined as the number of weeks since January the 6th
    1980.

    Epoch of gps weeks is 1980-01-06 00:00, which in modified Julian
    date is 44244.
************************************************************************/
    return (mjd - 44244.0) / 7.0;
}

struct TIME_UNITS time_units[] = {
    {"mjd",         mjd_to_mjd,         mjd_to_mjd,         "Modified julian date"},
    {"decimalyear", decimalyear_to_mjd, mjd_to_decimalyear, "Decimal year"},
    {"gps_week",    gps_week_to_mjd,    mjd_to_gps_week,    "GPS Week"},
    {NULL,          NULL,               NULL,               NULL}
};

struct pj_opaque_unitconvert {
    int     t_in_id;    /* time unit id for the time input unit   */
    int     t_out_id;   /* time unit id for the time output unit  */
    int     xy_in_id;   /* unit id for the horizontal input unit  */
    int     xy_out_id;  /* unit id for the horizontal output unit */
    int     z_in_id;    /* unit id for the vertical input unit    */
    int     z_out_id;   /* unit id for the vertical output unit   */
};


/***********************************************************************/
static void *freeup_msg (PJ *P, int errlev) {
/***********************************************************************/
    if (0==P)
        return 0;

    if (0!=P->ctx)
        pj_ctx_set_errno (P->ctx, errlev);

    pj_dealloc (P->opaque);

    return pj_dealloc(P);
}


/***********************************************************************/
static void freeup (PJ *P) {
/***********************************************************************/
    freeup_msg (P, 0);
    return;
}

/***********************************************************************/
static XY forward_2d(LP lp, PJ *P) {
/************************************************************************
    Forward unit conversions in the plane
************************************************************************/
    struct pj_opaque_unitconvert *Q = (struct pj_opaque_unitconvert *) P->opaque;
    PJ_TRIPLET point;
    point.lp = lp;

    point.xy.x *= pj_units[Q->xy_in_id].factor / pj_units[Q->xy_out_id].factor;
    point.xy.y *= pj_units[Q->xy_in_id].factor / pj_units[Q->xy_out_id].factor;

    return point.xy;
}


/***********************************************************************/
static LP reverse_2d(XY xy, PJ *P) {
/************************************************************************
    Reverse unit conversions in the plane
************************************************************************/
    struct pj_opaque_unitconvert *Q = (struct pj_opaque_unitconvert *) P->opaque;
    PJ_TRIPLET point;
    point.xy = xy;

    point.xy.x *= pj_units[Q->xy_out_id].factor / pj_units[Q->xy_in_id].factor;
    point.xy.y *= pj_units[Q->xy_out_id].factor / pj_units[Q->xy_in_id].factor;

    return point.lp;
}


/***********************************************************************/
static XYZ forward_3d(LPZ lpz, PJ *P) {
/************************************************************************
    Forward unit conversions the vertical component
************************************************************************/
    struct pj_opaque_unitconvert *Q = (struct pj_opaque_unitconvert *) P->opaque;
    PJ_TRIPLET point;
    point.lpz = lpz;

    /* take care of the horizontal components in the 2D function */
    point.xy = forward_2d(point.lp, P);

    point.xyz.z *= pj_units[Q->z_in_id].factor / pj_units[Q->z_out_id].factor;

    return point.xyz;
}

/***********************************************************************/
static LPZ reverse_3d(XYZ xyz, PJ *P) {
/************************************************************************
    Reverse unit conversions the vertical component
************************************************************************/
    struct pj_opaque_unitconvert *Q = (struct pj_opaque_unitconvert *) P->opaque;
    PJ_TRIPLET point;
    point.xyz = xyz;

    /* take care of the horizontal components in the 2D function */
    point.lp = reverse_2d(point.xy, P);

    point.xyz.z *= pj_units[Q->z_out_id].factor / pj_units[Q->z_in_id].factor;

    return point.lpz;
}


/***********************************************************************/
static PJ_OBS forward_obs(PJ_OBS obs, PJ *P) {
/************************************************************************
    Forward conversion of time units
************************************************************************/
    struct pj_opaque_unitconvert *Q = (struct pj_opaque_unitconvert *) P->opaque;
    PJ_OBS out;

    /* delegate unit conversion of physical dimensions to the 3D function */
    out.coo.xyz = forward_3d(obs.coo.lpz, P);

    if (Q->t_in_id >= 0)
        out.coo.xyzt.t = time_units[Q->t_in_id].t_in( obs.coo.xyzt.t );
    if (Q->t_out_id >= 0)
        out.coo.xyzt.t = time_units[Q->t_out_id].t_out( out.coo.xyzt.t );

    return out;
}


/***********************************************************************/
static PJ_OBS reverse_obs(PJ_OBS obs, PJ *P) {
/************************************************************************
    Reverse conversion of time units
************************************************************************/
    struct pj_opaque_unitconvert *Q = (struct pj_opaque_unitconvert *) P->opaque;
    PJ_OBS out;

    /* delegate unit conversion of physical dimensions to the 3D function */
    out.coo.lpz = reverse_3d(obs.coo.xyz, P);

    if (Q->t_in_id >= 0)
        out.coo.xyzt.t = time_units[Q->t_out_id].t_in( obs.coo.xyzt.t );
    if (Q->t_out_id >= 0)
        out.coo.xyzt.t = time_units[Q->t_in_id].t_out( out.coo.xyzt.t );

    return out;
}


/***********************************************************************/
PJ *PROJECTION(unitconvert) {
/***********************************************************************/
    struct pj_opaque_unitconvert *Q = pj_calloc (1, sizeof (struct pj_opaque_unitconvert));
    char *s, *name;
    int i;

    if (0==Q)
        return freeup_msg (P, ENOMEM);
    P->opaque = (void *) Q;

    P->fwdobs = forward_obs;
    P->invobs = reverse_obs;
    P->fwd3d  = forward_3d;
    P->inv3d  = reverse_3d;
    P->fwd    = forward_2d;
    P->inv    = reverse_2d;

    P->left  = PJ_IO_UNITS_RADIANS;
    P->right = PJ_IO_UNITS_RADIANS;

    /* if no time input/output unit is specified we can skip them */
    Q->t_in_id = -1;
    Q->t_out_id = -1;

    if ((name = pj_param (P->ctx, P->params, "sxy_in").s) != NULL) {
        for (i = 0; (s = pj_units[i].id) && strcmp(name, s) ; ++i);

        if (!s) return freeup_msg(P, -8); /* unknown unit conversion id */

        Q->xy_in_id = i;
        proj_log_debug(P, "xy_in unit: %s", pj_units[i].name);
    }

    if ((name = pj_param (P->ctx, P->params, "sxy_out").s) != NULL) {
        for (i = 0; (s = pj_units[i].id) && strcmp(name, s) ; ++i);

        if (!s) return freeup_msg(P, -8); /* unknown unit conversion id */

        Q->xy_out_id = i;
        proj_log_debug(P, "xy_out unit: %s", pj_units[i].name);
    }

    if ((name = pj_param (P->ctx, P->params, "sz_in").s) != NULL) {
        for (i = 0; (s = pj_units[i].id) && strcmp(name, s) ; ++i);

        if (!s) return freeup_msg(P, -8); /* unknown unit conversion id */

        Q->z_in_id = i;
        proj_log_debug(P, "z_in unit: %s", pj_units[i].name);
    }

    if ((name = pj_param (P->ctx, P->params, "sz_out").s) != NULL) {
        for (i = 0; (s = pj_units[i].id) && strcmp(name, s) ; ++i);

        if (!s) return freeup_msg(P, -8); /* unknown unit conversion id */

        Q->z_out_id = i;
        proj_log_debug(P, "z_out unit: %s", pj_units[i].name);
    }


    if ((name = pj_param (P->ctx, P->params, "st_in").s) != NULL) {
        for (i = 0; (s = time_units[i].id) && strcmp(name, s) ; ++i);

        if (!s) return freeup_msg(P, -8); /* unknown unit conversion id */

        Q->t_in_id = i;
        proj_log_debug(P, "t_in unit: %s", time_units[i].name);
    }

    s = 0;
    if ((name = pj_param (P->ctx, P->params, "st_out").s) != NULL) {
        for (i = 0; (s = time_units[i].id) && strcmp(name, s) ; ++i);
        if (!s) {
            return freeup_msg(P, -8); /* unknown unit conversion id */
        }
        Q->t_out_id = i;
        proj_log_debug(P, "t_out unit: %s", time_units[i].name);
    }

    return P;
}

#ifndef PJ_SELFTEST

int pj_unitconvert_selftest (void) {return 0;}

#else

static int test_time(char* args, double tol, double t_in, double t_exp) {
    PJ_OBS in, out;
    PJ *P = proj_create(0, args);
    int ret = 0;

    if (P == 0)
        return 5;

    in.coo.xyzt.t = t_in;

    out = proj_trans_obs(P, PJ_FWD, in);
    if (fabs(out.coo.xyzt.t - t_exp) > tol) {
        proj_log_error(P, "out: %10.10g, expect: %10.10g", out.coo.xyzt.t, t_exp);
        ret = 1;
    }
    out = proj_trans_obs(P, PJ_INV, out);
    if (fabs(out.coo.xyzt.t - t_in) > tol) {
        proj_log_error(P, "out: %10.10g, expect: %10.10g", out.coo.xyzt.t, t_in);
        ret = 2;
    }
    pj_free(P);

    proj_log_level(NULL, 0);
    return ret;
}

static int test_xyz(char* args, double tol, PJ_TRIPLET in, PJ_TRIPLET exp) {
    PJ_OBS out, obs_in;
    PJ *P = proj_create(0, args);
    int ret = 0;

    if (P == 0)
        return 5;

    obs_in.coo.xyz = in.xyz;
    out = proj_trans_obs(P, PJ_FWD, obs_in);
    if (proj_xyz_dist(out.coo.xyz, exp.xyz) > tol) {
        printf("exp: %10.10g, %10.10g, %10.10g\n", exp.xyz.x, exp.xyz.y, exp.xyz.z);
        printf("out: %10.10g, %10.10g, %10.10g\n", out.coo.xyz.x, out.coo.xyz.y, out.coo.xyz.z);
        ret = 1;
    }

    out = proj_trans_obs(P, PJ_INV, out);
    if (proj_xyz_dist(out.coo.xyz, in.xyz) > tol) {
        printf("exp: %g, %g, %g\n", in.xyz.x, in.xyz.y, in.xyz.z);
        printf("out: %g, %g, %g\n", out.coo.xyz.x, out.coo.xyz.y, out.coo.xyz.z);
        ret += 2;
    }
    proj_destroy(P);
    proj_log_level(NULL, 0);
    return ret;
}


int pj_unitconvert_selftest (void) {
    int ret = 0;
    char args1[] = "+proj=unitconvert +t_in=decimalyear +t_out=decimalyear";
    double in1 = 2004.25;

    char args2[] = "+proj=unitconvert +t_in=gps_week +t_out=gps_week";
    double in2 = 1782.0;

    char args3[] = "+proj=unitconvert +t_in=mjd +t_out=mjd";
    double in3 = 57390.0;

    char args4[] = "+proj=unitconvert +t_in=gps_week +t_out=decimalyear";
    double in4 = 1877.71428, exp4 = 2016.0;

    char args5[] = "+proj=unitconvert +xy_in=m +xy_out=dm +z_in=cm +z_out=mm";
    PJ_TRIPLET in5 = {{55.25, 23.23, 45.5}}, exp5 = {{552.5, 232.3, 455.0}};

    char args6[] = "+proj=unitconvert +xy_in=m +xy_out=m +z_in=m +z_out=m";
    PJ_TRIPLET in6 = {{12.3, 45.6, 7.89}};

    ret = test_time(args1, 1e-6, in1, in1);   if (ret) return ret + 10;
    ret = test_time(args2, 1e-6, in2, in2);   if (ret) return ret + 20;
    ret = test_time(args3, 1e-6, in3, in3);   if (ret) return ret + 30;
    ret = test_time(args4, 1e-6, in4, exp4);  if (ret) return ret + 40;
    ret = test_xyz (args5, 1e-10, in5, exp5); if (ret) return ret + 50;
    ret = test_xyz (args6, 1e-10, in6, in6);  if (ret) return ret + 50;

    return 0;

}

#endif
