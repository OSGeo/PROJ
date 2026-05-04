/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Higher-level 3D vector helpers for spherical geometry.
 * Author:   Felix Palmer
 *
 ******************************************************************************
 * Derived from A5 (Apache-2.0).
 * https://github.com/felixpalmer/a5
 * 
 * modules/utils/vector.ts
 *
 * Copyright (c) A5 contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ****************************************************************************/

#ifndef VEC3_HELPERS_H
#define VEC3_HELPERS_H

#include "vec3.h"

#include <algorithm>
#include <cmath>

namespace polyhedral {

// Numerically stable angular distance measure.
// Returns a value proportional to sin(angle/2) between a and b.
inline double vector_difference(const Vec3 &a, const Vec3 &b) {
    Vec3 midpoint_ab = vec3_normalize(vec3_lerp(a, b, 0.5));
    Vec3 cross_result = vec3_cross(a, midpoint_ab);
    double d = vec3_length(cross_result);

    if (d < 1e-8) {
        // When A and B are very close, sin(x/2) ≈ x/2
        return 0.5 * vec3_length(vec3_subtract(a, b));
    }
    return d;
}

inline double triple_product(const Vec3 &a, const Vec3 &b, const Vec3 &c) {
    return vec3_dot(a, vec3_cross(b, c));
}

// Great-circle intersection helper.
// Returns dot(b,cross(c,d))*a - dot(a,cross(c,d))*b
inline Vec3 quadruple_product(const Vec3 &a, const Vec3 &b, const Vec3 &c,
                              const Vec3 &d) {
    Vec3 cross_cd = vec3_cross(c, d);
    double tp_acd = vec3_dot(a, cross_cd);
    double tp_bcd = vec3_dot(b, cross_cd);
    return vec3_subtract(vec3_scale(b, tp_acd), vec3_scale(a, tp_bcd));
}

inline Vec3 slerp(const Vec3 &a, const Vec3 &b, double t) {
    double gamma = vec3_angle(a, b);
    if (gamma < 1e-12) {
        return vec3_lerp(a, b, t);
    }
    double sin_gamma = std::sin(gamma);
    double wa = std::sin((1.0 - t) * gamma) / sin_gamma;
    double wb = std::sin(t * gamma) / sin_gamma;
    return vec3_add(vec3_scale(a, wa), vec3_scale(b, wb));
}

// Spherical triangle area via midpoint triple product method.
// More numerically stable than vertex-based spherical excess.
inline double spherical_triangle_area(const Vec3 &v1, const Vec3 &v2,
                                      const Vec3 &v3) {
    Vec3 mid_a = vec3_normalize(vec3_lerp(v2, v3, 0.5));
    Vec3 mid_b = vec3_normalize(vec3_lerp(v3, v1, 0.5));
    Vec3 mid_c = vec3_normalize(vec3_lerp(v1, v2, 0.5));
    double s = triple_product(mid_a, mid_b, mid_c);
    s = std::clamp(s, -1.0, 1.0);
    if (std::fabs(s) < 1e-8) {
        return 2.0 * s;
    }
    return 2.0 * std::asin(s);
}

// Computes acos(1 - 2*x*x) without precision loss for small x.
inline double safe_acos(double x) {
    if (x < 1e-3) {
        return 2.0 * x + x * x * x / 3.0;
    }
    return std::acos(1.0 - 2.0 * x * x);
}

} // namespace polyhedral

#endif // VEC3_HELPERS_H
