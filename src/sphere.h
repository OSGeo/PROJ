/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Spherical geometry helpers on 3D unit vectors —
 *           slerp, spherical triangle area, scalar triple/quadruple
 *           products for great-circle math.
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

#ifndef SPHERE_H
#define SPHERE_H

#include "vec3.h"

#include <cmath>

// All routines in this header operate on *unit* vectors (points on the unit
// sphere); the identities they rely on assume |v| == 1 and results are
// undefined for non-normalized input.

// Numerically stable angular distance measure.
// Returns a value proportional to sin(angle/2) between a and b.
inline double vector_difference(const Vec3 &a, const Vec3 &b) {
    double D = vec3_length(vec3_subtract(a, b));
    double S = vec3_length(vec3_add(a, b));
    return D / std::hypot(D, S);
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
    if (gamma == 0.0) {
        return vec3_lerp(a, b, t);
    }
    double sin_gamma = std::sin(gamma);
    double wa = std::sin((1.0 - t) * gamma) / sin_gamma;
    double wb = std::sin(t * gamma) / sin_gamma;
    return vec3_add(vec3_scale(a, wa), vec3_scale(b, wb));
}

// Signed spherical excess (area) of triangle (v1, v2, v3) of unit vectors,
// See: https://brsr.github.io/2021/05/01/vector-spherical-geometry.html
inline double spherical_triangle_area(const Vec3 &v1, const Vec3 &v2,
                                      const Vec3 &v3) {
    double num = triple_product(v1, v2, v3);
    double den = 1.0 + vec3_dot(v1, v2) + vec3_dot(v2, v3) + vec3_dot(v3, v1);
    return 2.0 * std::atan2(num, den);
}

#endif // SPHERE_H
