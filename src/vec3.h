/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  3D vector operations for polyhedral projections.
 * Author:   Felix Palmer
 *
 ******************************************************************************
 * Derived from gl-matrix (MIT License).
 *
 * gl-matrix: Copyright (c) 2015-2021, Brandon Jones, Colin MacKenzie IV.
 * https://github.com/toji/gl-matrix
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
 ****************************************************************************/

#ifndef VEC3_H
#define VEC3_H

#include <algorithm>
#include <cmath>

struct Vec3 {
    double x = 0.0, y = 0.0, z = 0.0;
};

inline double vec3_dot(const Vec3 &a, const Vec3 &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3 vec3_cross(const Vec3 &a, const Vec3 &b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x};
}

inline double vec3_length(const Vec3 &v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

inline Vec3 vec3_normalize(const Vec3 &v) {
    double len = vec3_length(v);
    if (len == 0.0)
        return v;
    return {v.x / len, v.y / len, v.z / len};
}

inline Vec3 vec3_subtract(const Vec3 &a, const Vec3 &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

inline Vec3 vec3_add(const Vec3 &a, const Vec3 &b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

inline Vec3 vec3_scale(const Vec3 &v, double s) {
    return {v.x * s, v.y * s, v.z * s};
}

inline Vec3 vec3_lerp(const Vec3 &a, const Vec3 &b, double t) {
    return {a.x + t * (b.x - a.x), a.y + t * (b.y - a.y),
            a.z + t * (b.z - a.z)};
}

inline double vec3_angle(const Vec3 &a, const Vec3 &b) {
    double d = vec3_dot(a, b) / (vec3_length(a) * vec3_length(b));
    return std::acos(std::clamp(d, -1.0, 1.0));
}

struct Mat4 {
    double m[4][4] = {{1.0, 0.0, 0.0, 0.0},
                      {0.0, 1.0, 0.0, 0.0},
                      {0.0, 0.0, 1.0, 0.0},
                      {0.0, 0.0, 0.0, 1.0}};
};

// Apply an affine 4x4 to a 3D point (treated as (v.x, v.y, v.z, 1)). Assumes
// the bottom row is [0, 0, 0, 1] (true affine, no perspective divide).
inline Vec3 vec3_apply_mat4(const Mat4 &m, const Vec3 &v) {
    return {
        m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3],
        m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3],
        m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3],
    };
}

#endif // VEC3_H
