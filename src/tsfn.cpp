/* determine small t */
#include <math.h>
#include "projects.h"

double pj_tsfn(double phi, double sinphi, double e) {
    double denominator;
    sinphi *= e;

    /* avoid zero division, fail gracefully */
    denominator = 1.0 + sinphi;
    if (denominator == 0.0)
        return HUGE_VAL;

    return (tan (.5 * (M_HALFPI - phi)) /
            pow((1. - sinphi) / (denominator), .5 * e));
}
