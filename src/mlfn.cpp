#include "proj_internal.h"
#include <math.h>

/* meridional distance for ellipsoid and inverse using 6th-order expansion in
** the third flattening n.  This gives full double precision accuracy for |f|
** <= 1/150.
*/

#define Lmax 6

// Evaluation sum(p[i] * x^i, i, 0, N) via Horner's method.  N.B. p is of
// length N+1.
static double polyval(double x, const double p[], int N) {
    double y = N < 0 ? 0 : p[N];
    for (; N > 0;)
        y = y * x + p[--N];
    return y;
}

// Evaluate y = sum(c[k] * sin((2*k+2) * zeta), k, 0, K-1)
static double clenshaw(double szeta, double czeta, const double c[], int K) {
    // Approx operation count = (K + 5) mult and (2 * K + 2) add
    double u0 = 0, u1 = 0,                         // accumulators for sum
        X = 2 * (czeta - szeta) * (czeta + szeta); // 2 * cos(2*zeta)
    for (; K > 0;) {
        double t = X * u0 - u1 + c[--K];
        u1 = u0;
        u0 = t;
    }
    return 2 * szeta * czeta * u0; // sin(2*zeta) * u0
}

double *pj_enfn(double n) {

    // Expansion of (quarter meridian) / ((a+b)/2 * pi/2) as series in n^2;
    // these coefficients are ( (2*k - 3)!! / (2*k)!! )^2 for k = 0..3
    static const double coeff_rad[] = {1, 1.0 / 4, 1.0 / 64, 1.0 / 256};

    // Coefficients to convert phi to mu, Eq. A5 in arXiv:2212.05818
    // with 0 terms dropped
    static const double coeff_mu_phi[] = {
        -3.0 / 2,    9.0 / 16,     -3.0 / 32,     15.0 / 16,
        -15.0 / 32,  135.0 / 2048, -35.0 / 48,    105.0 / 256,
        315.0 / 512, -189.0 / 512, -693.0 / 1280, 1001.0 / 2048,
    };
    // Coefficients to convert mu to phi, Eq. A6 in arXiv:2212.05818
    // with 0 terms dropped
    static const double coeff_phi_mu[] = {
        3.0 / 2,      -27.0 / 32,      269.0 / 512,   21.0 / 16,
        -55.0 / 32,   6759.0 / 4096,   151.0 / 96,    -417.0 / 128,
        1097.0 / 512, -15543.0 / 2560, 8011.0 / 2560, 293393.0 / 61440,
    };

    double n2 = n * n, d = n, *en;

    // 2*Lmax for the Fourier coeffs for each direction of conversion + 1 for
    // overall multiplier.
    en = (double *)malloc((2 * Lmax + 1) * sizeof(double));
    if (nullptr == en)
        return nullptr;
    en[0] = polyval(n2, coeff_rad, Lmax / 2) / (1 + n);
    for (int l = 0, o = 0; l < Lmax; ++l) {
        int m = (Lmax - l - 1) / 2;
        en[l + 1] = d * polyval(n2, coeff_mu_phi + o, m);
        en[l + 1 + Lmax] = d * polyval(n2, coeff_phi_mu + o, m);
        d *= n;
        o += m + 1;
    }
    return en;
}

double pj_mlfn(double phi, double sphi, double cphi, const double *en) {
    return en[0] * (phi + clenshaw(sphi, cphi, en + 1, Lmax));
}

double pj_inv_mlfn(double mu, const double *en) {
    mu /= en[0];
    return mu + clenshaw(sin(mu), cos(mu), en + 1 + Lmax, Lmax);
}
