#define PJ_LIB_

#include <math.h>

#include "proj.h"
#include "proj_internal.h"

PROJ_HEAD(wag7, "Wagner VII") "\n\tMisc Sph";

/*
These formulae are from https://github.com/d3/d3-geo-projection/blob/main/src/wagner7.js.
The original code is under the ISC License, which is permissive.
See https://github.com/d3/d3/blob/main/LICENSE.
The constants are explained at
http://www.boehmwanderkarten.de/kartographie/is_netze_wagner_123456789_inversions.html.

Required license text:

Copyright 2013-2021 Mike Bostock

Permission to use, copy, modify, and/or distribute this software for any purpose
with or without fee is hereby granted, provided that the above copyright notice
and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.
*/

static PJ_XY wag7_s_forward (PJ_LP lp, PJ *P) {           /* Spheroidal, forward */
    (void)P;
    PJ_XY xy = { 0, 0 };
    double s = 0.90630778703664994 * sin(lp.phi);
    double c0 = sqrt(1 - s * s);
    double c1 = sqrt(2 / (1 + c0 * cos(lp.lam / 3.0)));
    xy.x = (5.3344669029266510 / 2.0) * c0 * c1 * sin(lp.lam / 3.0);
    xy.y = (2.4820727672498521 / 2.0) * s * c1;
    return xy;
}

static PJ_LP wag7_s_inverse (PJ_XY xy, PJ *P) {
    (void)P;
    PJ_LP lp = { 0, 0 };
    double t1 = xy.x / (5.3344669029266510 / 2.0);
    double t2 = xy.y / (2.4820727672498521 / 2.0);
    double p = sqrt(t1 * t1 + t2 * t2);
    double c = 2 * asin(p / 2);
    lp.lam = 3 * atan2(xy.x * tan(c),(5.3344669029266510 / 2.0) * p);
    lp.phi = p ? asin(xy.y * sin(c) / ((2.4820727672498521 / 2.0) * 0.90630778703664994 * p)) : 0;
    return lp;    
}

PJ *PROJECTION(wag7) {
    P->fwd = wag7_s_forward;
    P->inv = wag7_s_inverse;
    P->es = 0.;
    return P;
}
