/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Primary (private) include file for PROJ.4 library.
 * Author:   Gerald Evenden
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
 ******************************************************************************
 *
 * $Log$
 * Revision 1.11  2002/06/20 16:09:31  warmerda
 * removed strtod, reimplement non-GPL strtod cover within dmstor.c
 *
 * Revision 1.10  2002/06/13 14:06:49  warmerda
 * Removed incorrect labelling of 3PARAM and 7PARAM as Molodensky.
 *
 * Revision 1.9  2001/04/06 01:24:13  warmerda
 * Introduced proj_api.h as a public interface for PROJ.4
 *
 * Revision 1.8  2001/04/05 04:24:10  warmerda
 * added prototypes for new functions, and PJ_VERSION
 *
 * Revision 1.7  2001/02/07 17:55:05  warmerda
 * Cleaned up various warnings when compiled with -Wall.
 *
 * Revision 1.6  2000/11/30 03:37:22  warmerda
 * use proj_strtod() in dmstor()
 *
 * Revision 1.5  2000/07/06 23:36:47  warmerda
 * added lots of datum related stuff
 *
 */

/* General projections header file */
#ifndef PROJECTS_H
#define PROJECTS_H

/* standard inclusions */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#  define NULL	0
#endif

#ifndef FALSE
#  define FALSE	0
#endif

#ifndef TRUE
#  define TRUE	1
#endif

#ifndef MAX
#  define MIN(a,b)      ((a<b) ? a : b)
#  define MAX(a,b)      ((a>b) ? a : b)
#endif

#ifndef ABS
#  define ABS(x)        ((x<0) ? (-1*(x)) : x)
#endif

    /* maximum path/filename */
#ifndef MAX_PATH_FILENAME
#define MAX_PATH_FILENAME 1024
#endif
	/* prototype hypot for systems where absent */
extern double hypot(double, double);
	/* some useful constants */
#define HALFPI		1.5707963267948966
#define FORTPI		0.78539816339744833
#define PI		3.14159265358979323846
#define TWOPI		6.2831853071795864769

/* environment parameter name */
#ifndef PROJ_LIB
#define PROJ_LIB "PROJ_LIB"
#endif
/* maximum tag id length for +init and default files */
#ifndef ID_TAG_MAX
#define ID_TAG_MAX 50
#endif

/* directory delimiter for DOS support */
#ifdef DOS
#define DIR_CHAR '\\'
#else
#define DIR_CHAR '/'
#endif

/* datum_type values */
#define PJD_UNKNOWN   0
#define PJD_3PARAM    1   
#define PJD_7PARAM    2   
#define PJD_GRIDSHIFT 3
#define PJD_WGS84     4   /* WGS84 (or anything considered equivelent) */

/* datum system errors */
#define PJD_ERR_GEOCENTRIC 100

#define USE_PROJUV 

typedef struct { double u, v; } projUV;
typedef struct { double r, i; }	COMPLEX;

#ifndef PJ_LIB__
#define XY projUV
#define LP projUV
#else
typedef struct { double x, y; }     XY;
typedef struct { double lam, phi; } LP;
#endif

typedef union { double  f; int  i; char *s; } PVALUE;

struct PJ_LIST {
	char	*id;		/* projection keyword */
	void	*(*proj)();	/* projection entry point */
	char 	* const *descr;	/* description text */
};
struct PJ_ELLPS {
	char	*id;	/* ellipse keyword name */
	char	*major;	/* a= value */
	char	*ell;	/* elliptical parameter */
	char	*name;	/* comments */
};
struct PJ_UNITS {
	char	*id;	/* units keyword */
	char	*to_meter;	/* multiply by value to get meters */
	char	*name;	/* comments */
};

struct PJ_DATUMS {
    char    *id;     /* datum keyword */
    char    *defn;   /* ie. "to_wgs84=..." */
    char    *ellipse_id; /* ie from ellipse table */
    char    *comments; /* EPSG code, etc */
};

struct FACTORS {
	struct DERIVS {
		double x_l, x_p; /* derivatives of x for lambda-phi */
		double y_l, y_p; /* derivatives of y for lambda-phi */
	} der;
	double h, k;	/* meridinal, parallel scales */
	double omega, thetap;	/* angular distortion, theta prime */
	double conv;	/* convergence */
	double s;		/* areal scale factor */
	double a, b;	/* max-min scale error */
	int code;		/* info as to analytics, see following */
};
#define IS_ANAL_XL_YL 01	/* derivatives of lon analytic */
#define IS_ANAL_XP_YP 02	/* derivatives of lat analytic */
#define IS_ANAL_HK	04		/* h and k analytic */
#define IS_ANAL_CONV 010	/* convergence analytic */
    /* parameter list struct */
typedef struct ARG_list {
	struct ARG_list *next;
	char used;
	char param[1]; } paralist;
	/* base projection data structure */


typedef struct PJconsts {
	XY  (*fwd)(LP, struct PJconsts *);
	LP  (*inv)(XY, struct PJconsts *);
	void (*spc)(LP, struct PJconsts *, struct FACTORS *);
	void (*pfree)(struct PJconsts *);
	const char *descr;
	paralist *params;   /* parameter list */
	int over;   /* over-range flag */
	int geoc;   /* geocentric latitude flag */
        int is_latlong; /* proj=latlong ... not really a projection at all */
	double
		a,  /* major axis or radius if es==0 */
		e,  /* eccentricity */
		es, /* e ^ 2 */
		ra, /* 1/A */
		one_es, /* 1 - e^2 */
		rone_es, /* 1/one_es */
		lam0, phi0, /* central longitude, latitude */
		x0, y0, /* easting and northing */
		k0,	/* general scaling factor */
		to_meter, fr_meter; /* cartesian scaling */

        int     datum_type; /* PJD_UNKNOWN/3PARAM/7PARAM/GRIDSHIFT/WGS84 */
        double  datum_params[7];
        
#ifdef PROJ_PARMS__
PROJ_PARMS__
#endif /* end of optional extensions */
} PJ;

/* public API */
#include "proj_api.h"

/* Generate pj_list external or make list from include file */
#ifndef PJ_LIST_H
extern struct PJ_LIST pj_list[];
#else
#define PROJ_HEAD(id, name) \
	extern void *pj_##id(); extern char * const pj_s_##id;
#ifndef lint
#define DO_PJ_LIST_ID
#endif
#include PJ_LIST_H
#ifndef lint
#undef DO_PJ_LIST_ID
#endif
#undef PROJ_HEAD
#define PROJ_HEAD(id, name) {#id, pj_##id, &pj_s_##id},
	struct PJ_LIST
pj_list[] = {
#include PJ_LIST_H
		{0,     0,  0},
	};
#undef PROJ_HEAD
#endif

#ifndef PJ_ELLPS__
extern struct PJ_ELLPS pj_ellps[];
#endif

#ifndef PJ_UNITS__
extern struct PJ_UNITS pj_units[];
#endif

#ifndef PJ_DATUMS__
extern struct PJ_DATUMS pj_datums[];
#endif

#ifdef PJ_LIB__
    /* repeatative projection code */
#define PROJ_HEAD(id, name) static const char des_##id [] = name
#define ENTRYA(name) const char * const pj_s_##name = des_##name; \
	PJ *pj_##name(PJ *P) { if (!P) { \
	if( (P = pj_malloc(sizeof(PJ))) != NULL) { \
	P->pfree = freeup; P->fwd = 0; P->inv = 0; \
	P->spc = 0; P->descr = des_##name;
#define ENTRYX } return P; } else {
#define ENTRY0(name) ENTRYA(name) ENTRYX
#define ENTRY1(name, a) ENTRYA(name) P->a = 0; ENTRYX
#define ENTRY2(name, a, b) ENTRYA(name) P->a = 0; P->b = 0; ENTRYX
#define ENDENTRY(p) } return (p); }
#define E_ERROR(err) { pj_errno = err; freeup(P); return(0); }
#define E_ERROR_0 { freeup(P); return(0); }
#define F_ERROR { pj_errno = -20; return(xy); }
#define I_ERROR { pj_errno = -20; return(lp); }
#define FORWARD(name) static XY name(LP lp, PJ *P) { XY xy
#define INVERSE(name) static LP name(XY xy, PJ *P) { LP lp
#define FREEUP static void freeup(PJ *P) {
#define SPECIAL(name) static void name(LP lp, PJ *P, struct FACTORS *fac)
#endif
#define MAX_TAB_ID 80
typedef struct { float lam, phi; } FLP;
typedef struct { int lam, phi; } ILP;
struct CTABLE {
	char id[MAX_TAB_ID]; /* ascii info */
	LP ll;      /* lower left corner coordinates */
	LP del;     /* size of cells */
	ILP lim;    /* limits of conversion matrix */
	FLP *cvs;   /* conversion matrix */
};
	/* procedure prototypes */
double dmstor(const char *, char **);
void set_rtodms(int, int);
char *rtodms(char *, double, int, int);
double adjlon(double);
double aacos(double), aasin(double), asqrt(double), aatan2(double, double);
PVALUE pj_param(paralist *, char *);
paralist *pj_mkparam(char *);
int pj_ell_set(paralist *, double *, double *);
int pj_datum_set(paralist *, PJ *);
double *pj_enfn(double);
double pj_mlfn(double, double, double, double *);
double pj_inv_mlfn(double, double, double *);
double pj_qsfn(double, double, double);
double pj_tsfn(double, double, double);
double pj_msfn(double, double, double);
double pj_phi2(double, double);
double pj_qsfn_(double, PJ *);
double *pj_authset(double);
double pj_authlat(double, double *);
COMPLEX pj_zpoly1(COMPLEX, COMPLEX *, int);
COMPLEX pj_zpolyd1(COMPLEX, COMPLEX *, int, COMPLEX *);
FILE *pj_open_lib(char *, char *);

int pj_deriv(LP, double, PJ *, struct DERIVS *);
int pj_factors(LP, PJ *, double, struct FACTORS *);

/* Approximation structures and procedures */
typedef struct {	/* Chebyshev or Power series structure */
	projUV a, b;		/* power series range for evaluation */
					/* or Chebyshev argument shift/scaling */
	struct PW_COEF {/* row coefficient structure */
		int m;		/* number of c coefficients (=0 for none) */
		double *c;	/* power coefficients */
	} *cu, *cv;
	int mu, mv;		/* maximum cu and cv index (+1 for count) */
	int power;		/* != 0 if power series, else Chebyshev */
} Tseries;
Tseries *mk_cheby(projUV, projUV, double, projUV *, projUV (*)(projUV), int, int, int);
projUV bpseval(projUV, Tseries *);
projUV bcheval(projUV, Tseries *);
projUV biveval(projUV, Tseries *);
void *vector1(int, int);
void **vector2(int, int, int);
int bchgen(projUV, projUV, int, int, projUV **, projUV(*)(projUV));
int bch2bps(projUV, projUV, projUV **, int, int);
/* nadcon related protos */
LP nad_intr(LP, struct CTABLE *);
LP nad_cvt(LP, int, struct CTABLE *);
struct CTABLE *nad_init(char *);
void nad_free(struct CTABLE *);
extern char const pj_release[];

#ifdef __cplusplus
}
#endif

#endif /* end of basic projections header */
