/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
/*
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#define PJ_LIB__

#include <math.h>
#include <stdlib.h>

#include "projects.h"

#define MAX_ITER 20

struct GAUSS {
    double C;
    double K;
    double e;
    double ratexp;
};
#define DEL_TOL 1e-14

static double srat(double esinp, double ratexp) {
    return(pow((1.-esinp)/(1.+esinp), ratexp));
}

void *pj_gauss_ini(double e, double phi0, double *chi, double *rc) {
    double sphi, cphi, es;
    struct GAUSS *en;

    if ((en = (struct GAUSS *)malloc(sizeof(struct GAUSS))) == NULL)
        return (NULL);
    es = e * e;
    en->e = e;
    sphi = sin(phi0);
    cphi = cos(phi0);  cphi *= cphi;
    *rc = sqrt(1. - es) / (1. - es * sphi * sphi);
    en->C = sqrt(1. + es * cphi * cphi / (1. - es));
    if (en->C == 0.0) {
        free(en);
        return NULL;
    }
    *chi = asin(sphi / en->C);
    en->ratexp = 0.5 * en->C * e;
    en->K = tan(.5 * *chi + M_FORTPI) / (
        pow(tan(.5 * phi0 + M_FORTPI), en->C) *
        srat(en->e * sphi, en->ratexp)  );
    return ((void *)en);
}

LP pj_gauss(projCtx ctx, LP elp, const void *data) {
    const struct GAUSS *en = (const struct GAUSS *)data;
    LP slp;
    (void) ctx;

    slp.phi = 2. * atan( en->K *
        pow(tan(.5 * elp.phi + M_FORTPI), en->C) *
        srat(en->e * sin(elp.phi), en->ratexp) ) - M_HALFPI;
    slp.lam = en->C * (elp.lam);
    return(slp);
}

LP pj_inv_gauss(projCtx ctx, LP slp, const void *data) {
    const struct GAUSS *en = (const struct GAUSS *)data;
    LP elp;
    double num;
    int i;

    elp.lam = slp.lam / en->C;
    num = pow(tan(.5 * slp.phi + M_FORTPI)/en->K, 1./en->C);
    for (i = MAX_ITER; i; --i) {
        elp.phi = 2. * atan(num * srat(en->e * sin(slp.phi), -.5 * en->e))
            - M_HALFPI;
        if (fabs(elp.phi - slp.phi) < DEL_TOL) break;
            slp.phi = elp.phi;
    }
    /* convergence failed */
    if (!i)
        pj_ctx_set_errno(ctx, PJD_ERR_NON_CONV_INV_MERI_DIST);
    return (elp);
}
