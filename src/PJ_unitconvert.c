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

The modified Julian date is chosen as the pivot unit since it has a
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
#include <errno.h>
#include "proj_internal.h"
#include "projects.h"

PROJ_HEAD(unitconvert, "Unit conversion");

typedef double (*tconvert)(double);

struct TIME_UNITS {
    char        *id;        /* units keyword */
    tconvert     t_in;      /* unit -> mod. julian date funtion pointer */
    tconvert     t_out;     /* mod. julian date > unit function pointer */
    char        *name;      /* comments */
};

struct pj_opaque_unitconvert {
    int     t_in_id;        /* time unit id for the time input unit   */
    int     t_out_id;       /* time unit id for the time output unit  */
    double  xy_factor;      /* unit conversion factor for horizontal components */
    double  z_factor;       /* unit conversion factor for vertical components */
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
static unsigned int days_in_month(unsigned int year, unsigned int month) {
/***********************************************************************/
    const unsigned int month_table[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    unsigned int days;

    if (month > 12) month = 12;
    if (month == 0) month = 1;

    days = month_table[month-1];
    if (is_leap_year(year) && month == 2) days++;

    return days;
}


/***********************************************************************/
static int daynumber_in_year(unsigned int year, unsigned int month, unsigned int day) {
/***********************************************************************/
    unsigned int daynumber=0, i;

    if (month > 12) month = 12;
    if (month == 0) month = 1;
    if (day > days_in_month(year, month)) day = days_in_month(year, month);

    for (i = 1; i < month; i++)
        daynumber += days_in_month(year, i);

    daynumber += day;

    return daynumber;

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
    int year;
    double fractional_year;
    double mjd;

    if( decimalyear < -10000 || decimalyear > 10000 )
        return 0;

    year = (int)floor(decimalyear);
    fractional_year = decimalyear - year;
    mjd = (year - 1859)*365 + 14 + 31;
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


/***********************************************************************/
static double yyyymmdd_to_mjd(double yyyymmdd) {
/************************************************************************
    Date given in YYYY-MM-DD format.
************************************************************************/

    int year  = (int)floor( yyyymmdd / 10000 );
    int month = (int)floor((yyyymmdd - year*10000) / 100);
    int day   = (int)floor( yyyymmdd - year*10000 - month*100);
    double mjd = daynumber_in_year(year, month, day);

    for (year -= 1; year > 1858; year--)
        mjd += days_in_year(year);

    return mjd + 13 + 31;
}


/***********************************************************************/
static double mjd_to_yyyymmdd(double mjd) {
/************************************************************************
    Date given in YYYY-MM-DD format.
************************************************************************/
    double mjd_iter = 14 + 31;
    int year = 1859, month=0, day=0;

    for (; mjd >= mjd_iter; year++) {
        mjd_iter += days_in_year(year);
    }
    year--;
    mjd_iter -= days_in_year(year);

    for (month=1; mjd_iter + days_in_month(year, month) <= mjd; month++)
        mjd_iter += days_in_month(year, month);

    day = (int)(mjd - mjd_iter + 1);

    return year*10000.0 + month*100.0 + day;
}

static const struct TIME_UNITS time_units[] = {
    {"mjd",         mjd_to_mjd,         mjd_to_mjd,         "Modified julian date"},
    {"decimalyear", decimalyear_to_mjd, mjd_to_decimalyear, "Decimal year"},
    {"gps_week",    gps_week_to_mjd,    mjd_to_gps_week,    "GPS Week"},
    {"yyyymmdd",    yyyymmdd_to_mjd,    mjd_to_yyyymmdd,    "YYYYMMDD date"},
    {NULL,          NULL,               NULL,               NULL}
};


/***********************************************************************/
static XY forward_2d(LP lp, PJ *P) {
/************************************************************************
    Forward unit conversions in the plane
************************************************************************/
    struct pj_opaque_unitconvert *Q = (struct pj_opaque_unitconvert *) P->opaque;
    PJ_COORD point = {{0,0,0,0}};
    point.lp = lp;

    point.xy.x *= Q->xy_factor;
    point.xy.y *= Q->xy_factor;

    return point.xy;
}


/***********************************************************************/
static LP reverse_2d(XY xy, PJ *P) {
/************************************************************************
    Reverse unit conversions in the plane
************************************************************************/
    struct pj_opaque_unitconvert *Q = (struct pj_opaque_unitconvert *) P->opaque;
    PJ_COORD point = {{0,0,0,0}};
    point.xy = xy;

    point.xy.x /= Q->xy_factor;
    point.xy.y /= Q->xy_factor;

    return point.lp;
}


/***********************************************************************/
static XYZ forward_3d(LPZ lpz, PJ *P) {
/************************************************************************
    Forward unit conversions the vertical component
************************************************************************/
    struct pj_opaque_unitconvert *Q = (struct pj_opaque_unitconvert *) P->opaque;
    PJ_COORD point = {{0,0,0,0}};
    point.lpz = lpz;

    /* take care of the horizontal components in the 2D function */
    point.xy = forward_2d(point.lp, P);

    point.xyz.z *= Q->z_factor;

    return point.xyz;
}

/***********************************************************************/
static LPZ reverse_3d(XYZ xyz, PJ *P) {
/************************************************************************
    Reverse unit conversions the vertical component
************************************************************************/
    struct pj_opaque_unitconvert *Q = (struct pj_opaque_unitconvert *) P->opaque;
    PJ_COORD point = {{0,0,0,0}};
    point.xyz = xyz;

    /* take care of the horizontal components in the 2D function */
    point.lp = reverse_2d(point.xy, P);

    point.xyz.z /= Q->z_factor;

    return point.lpz;
}


/***********************************************************************/
static PJ_COORD forward_4d(PJ_COORD obs, PJ *P) {
/************************************************************************
    Forward conversion of time units
************************************************************************/
    struct pj_opaque_unitconvert *Q = (struct pj_opaque_unitconvert *) P->opaque;
    PJ_COORD out = obs;

    /* delegate unit conversion of physical dimensions to the 3D function */
    out.xyz = forward_3d(obs.lpz, P);

    if (Q->t_in_id >= 0)
        out.xyzt.t = time_units[Q->t_in_id].t_in( obs.xyzt.t );
    if (Q->t_out_id >= 0)
        out.xyzt.t = time_units[Q->t_out_id].t_out( out.xyzt.t );

    return out;
}


/***********************************************************************/
static PJ_COORD reverse_4d(PJ_COORD obs, PJ *P) {
/************************************************************************
    Reverse conversion of time units
************************************************************************/
    struct pj_opaque_unitconvert *Q = (struct pj_opaque_unitconvert *) P->opaque;
    PJ_COORD out = obs;

    /* delegate unit conversion of physical dimensions to the 3D function */
    out.lpz = reverse_3d(obs.xyz, P);

    if (Q->t_out_id >= 0)
        out.xyzt.t = time_units[Q->t_out_id].t_in( obs.xyzt.t );
    if (Q->t_in_id >= 0)
        out.xyzt.t = time_units[Q->t_in_id].t_out( out.xyzt.t );

    return out;
}


/***********************************************************************/
PJ *CONVERSION(unitconvert,0) {
/***********************************************************************/
    struct pj_opaque_unitconvert *Q = pj_calloc (1, sizeof (struct pj_opaque_unitconvert));
    char *s, *name;
    int i;
    double f;

    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = (void *) Q;

    P->fwd4d  = forward_4d;
    P->inv4d  = reverse_4d;
    P->fwd3d  = forward_3d;
    P->inv3d  = reverse_3d;
    P->fwd    = forward_2d;
    P->inv    = reverse_2d;

    P->left  = PJ_IO_UNITS_WHATEVER;
    P->right = PJ_IO_UNITS_WHATEVER;

    /* if no time input/output unit is specified we can skip them */
    Q->t_in_id = -1;
    Q->t_out_id = -1;

    Q->xy_factor = 1.0;
    Q->z_factor  = 1.0;

    if ((name = pj_param (P->ctx, P->params, "sxy_in").s) != NULL) {
        for (i = 0; (s = pj_units[i].id) && strcmp(name, s) ; ++i);

        if (s) {
            f = pj_units[i].factor;
            proj_log_debug(P, "xy_in unit: %s", pj_units[i].name);
        } else {
            if ( (f = pj_param (P->ctx, P->params, "dxy_in").f) == 0.0)
                return pj_default_destructor(P, PJD_ERR_UNKNOW_UNIT_ID);
        }
        if (f != 0.0)
            Q->xy_factor *= f;
    }

    if ((name = pj_param (P->ctx, P->params, "sxy_out").s) != NULL) {
        for (i = 0; (s = pj_units[i].id) && strcmp(name, s) ; ++i);

        if (s) {
            f = pj_units[i].factor;
            proj_log_debug(P, "xy_out unit: %s", pj_units[i].name);
        } else {
            if ( (f = pj_param (P->ctx, P->params, "dxy_out").f) == 0.0)
                return pj_default_destructor(P, PJD_ERR_UNKNOW_UNIT_ID);
        }
        if (f != 0.0)
            Q->xy_factor /= f;
    }

    if ((name = pj_param (P->ctx, P->params, "sz_in").s) != NULL) {
        for (i = 0; (s = pj_units[i].id) && strcmp(name, s) ; ++i);

        if (s) {
            f = pj_units[i].factor;
            proj_log_debug(P, "z_in unit: %s", pj_units[i].name);
        } else {
            if ( (f = pj_param (P->ctx, P->params, "dz_in").f) == 0.0)
                return pj_default_destructor(P, PJD_ERR_UNKNOW_UNIT_ID);
        }
        if (f != 0.0)
            Q->z_factor *= f;
    }

    if ((name = pj_param (P->ctx, P->params, "sz_out").s) != NULL) {
        for (i = 0; (s = pj_units[i].id) && strcmp(name, s) ; ++i);

        if (s) {
            f = pj_units[i].factor;
            proj_log_debug(P, "z_out unit: %s", pj_units[i].name);
        } else {
            if ( (f = pj_param (P->ctx, P->params, "dz_out").f) == 0.0)
                return pj_default_destructor(P, PJD_ERR_UNKNOW_UNIT_ID);
        }
        if (f != 0.0)
            Q->z_factor /= f;
    }


    if ((name = pj_param (P->ctx, P->params, "st_in").s) != NULL) {
        for (i = 0; (s = time_units[i].id) && strcmp(name, s) ; ++i);

        if (!s) return pj_default_destructor(P, PJD_ERR_UNKNOW_UNIT_ID); /* unknown unit conversion id */

        Q->t_in_id = i;
        proj_log_debug(P, "t_in unit: %s", time_units[i].name);
    }

    s = 0;
    if ((name = pj_param (P->ctx, P->params, "st_out").s) != NULL) {
        for (i = 0; (s = time_units[i].id) && strcmp(name, s) ; ++i);

        if (!s) return pj_default_destructor(P, PJD_ERR_UNKNOW_UNIT_ID); /* unknown unit conversion id */

        Q->t_out_id = i;
        proj_log_debug(P, "t_out unit: %s", time_units[i].name);
    }

    return P;
}
