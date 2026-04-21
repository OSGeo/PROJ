/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Snyder equal-area polyhedral projection — forward and inverse.
 * Author:   Felix Palmer
 *
 ******************************************************************************
 * Derived from A5 (Apache-2.0).
 *
 * https://github.com/felixpalmer/a5
 * 
 * modules/projections/polyhedral.ts

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
 ****************************************************************************
 * Derived from DGGAL (BSD 3-Clause License)
 * 
 * Copyright (c) 2014-2025, Ecere Corporation
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************
 *
 * Closed from equations adapted from:
 * https://brsr.github.io/2021/08/31/snyder-equal-area.html
 *
 ****************************************************************************/

#ifndef SNYDER_H
#define SNYDER_H

#include "vec3.h"
#include "vec3_helpers.h"

namespace polyhedral {

struct Face2D {
    double x, y;
};

struct Barycentric {
    double u, v, w;
};

struct FaceTriangle {
    Face2D a, b, c;
};

struct SphericalTriangle {
    Vec3 a, b, c;
};

inline Barycentric face_to_barycentric(Face2D p, FaceTriangle tri) {
    double d31[2] = {tri.a.x - tri.c.x, tri.a.y - tri.c.y};
    double d23[2] = {tri.c.x - tri.b.x, tri.c.y - tri.b.y};
    double d3p[2] = {p.x - tri.c.x, p.y - tri.c.y};

    double det = d23[0] * d31[1] - d23[1] * d31[0];
    double b0 = (d23[0] * d3p[1] - d23[1] * d3p[0]) / det;
    double b1 = (d31[0] * d3p[1] - d31[1] * d3p[0]) / det;
    double b2 = 1.0 - (b0 + b1);
    return {b0, b1, b2};
}

inline Face2D barycentric_to_face(Barycentric b, FaceTriangle tri) {
    return {b.u * tri.a.x + b.v * tri.b.x + b.w * tri.c.x,
            b.u * tri.a.y + b.v * tri.b.y + b.w * tri.c.y};
}

inline Face2D snyder_fwd(Vec3 v, SphericalTriangle stri,
                                 FaceTriangle ftri) {
    Vec3 a = stri.a, b = stri.b, c = stri.c;

    Vec3 z = vec3_normalize(vec3_subtract(v, a));
    Vec3 p = vec3_normalize(quadruple_product(a, z, b, c));

    // Ensure p is on the same hemisphere as b and c (the quadruple product
    // may return the antipodal intersection depending on vertex winding)
    if (vec3_dot(p, b) + vec3_dot(p, c) < 0) {
        p = {-p.x, -p.y, -p.z};
    }

    double h = vector_difference(a, v) / vector_difference(a, p);
    double area_abc = spherical_triangle_area(a, b, c);
    double scaled_area = h / area_abc;
    double area_apc = spherical_triangle_area(a, p, c);
    double area_abp = spherical_triangle_area(a, b, p);

    Barycentric bary = {1.0 - h,
                        scaled_area * area_apc,
                        scaled_area * area_abp};
    return barycentric_to_face(bary, ftri);
}

inline Vec3 snyder_inv(Face2D face_point, FaceTriangle ftri,
                               SphericalTriangle stri) {
    Vec3 a = stri.a, b = stri.b, c = stri.c;
    Barycentric bcoords = face_to_barycentric(face_point, ftri);

    constexpr double threshold = 1.0 - 1e-14;
    if (bcoords.u > threshold)
        return a;
    if (bcoords.v > threshold)
        return b;
    if (bcoords.w > threshold)
        return c;

    Vec3 c1 = vec3_cross(b, c);
    double area_abc = spherical_triangle_area(a, b, c);
    double h = 1.0 - bcoords.u;
    double r = bcoords.w / h;
    double alpha = r * area_abc;
    double s = std::sin(alpha);
    double half_c = std::sin(alpha / 2.0);
    double cc = 2.0 * half_c * half_c; // 1 - cos(alpha)

    double c01 = vec3_dot(a, b);
    double c12 = vec3_dot(b, c);
    double c20 = vec3_dot(c, a);
    double s12 = vec3_length(c1);

    double tp = vec3_dot(a, c1); // triple product A·(B×C)
    double f = s * tp + cc * (c01 * c12 - c20);
    double g = cc * s12 * (1.0 + c01);
    double q = (2.0 / std::acos(c12)) * std::atan2(g, f);
    Vec3 pp = slerp(b, c, q);
    double k = vector_difference(a, pp);
    double t = safe_acos(h * k) / safe_acos(k);
    return slerp(a, pp, t);
}

} // namespace polyhedral

#endif // SNYDER_H
