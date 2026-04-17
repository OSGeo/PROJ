/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Authalic latitude conversion via Clenshaw summation (order 6).
 * Author:   Felix Palmer
 *
 ******************************************************************************
 * Coefficients from Karney (2022): https://arxiv.org/pdf/2212.05818
 *
 * Clenshaw summation adapted from DGGAL project (authalic.ec).
 *
 * BSD 3-Clause License
 *
 * Copyright (c) 2014-2025, Ecere Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************/

#ifndef AUTHALIC_H
#define AUTHALIC_H

#include <cmath>

namespace polyhedral {

constexpr double GEODETIC_TO_AUTHALIC[6] = {
    -2.2392098386786394e-03,
     2.1308606513250217e-06,
    -2.5592576864212742e-09,
     3.3701965267802837e-12,
    -4.6675453126112487e-15,
     6.6749287038481596e-18,
};

constexpr double AUTHALIC_TO_GEODETIC[6] = {
     2.2392089963541657e-03,
     2.8831978048607556e-06,
     5.0862207399726603e-09,
     1.0201812377816100e-11,
     2.1912872306767718e-14,
     4.9284235482523806e-17,
};

inline double apply_clenshaw(double phi, const double c[6]) {
    double sin_phi = std::sin(phi);
    double cos_phi = std::cos(phi);
    double x = 2.0 * (cos_phi - sin_phi) * (cos_phi + sin_phi); // cos(2*phi)

    double u0 = x * c[5] + c[4];
    double u1 = x * u0 + c[3];
    u0 = x * u1 - u0 + c[2];
    u1 = x * u0 - u1 + c[1];
    u0 = x * u1 - u0 + c[0];

    return phi + 2.0 * sin_phi * cos_phi * u0;
}

inline double geodetic_to_authalic(double phi) {
    return apply_clenshaw(phi, GEODETIC_TO_AUTHALIC);
}

inline double authalic_to_geodetic(double phi) {
    return apply_clenshaw(phi, AUTHALIC_TO_GEODETIC);
}

} // namespace polyhedral

#endif // AUTHALIC_H
