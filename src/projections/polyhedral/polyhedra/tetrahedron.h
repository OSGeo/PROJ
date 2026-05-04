/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Regular tetrahedron inscribed in the unit sphere.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef POLYHEDRA_TETRAHEDRON_H
#define POLYHEDRA_TETRAHEDRON_H

#include "../vec3.h"

namespace polyhedra {
namespace tetrahedron {

// Following definition from https://dmccooey.com/polyhedra/Tetrahedron.txt
constexpr double C0 = 0.35355339059327376220042e+00; // sqrt(2) / 4

constexpr polyhedral::Vec3 VERTICES[4] = {
    {C0, -C0, C0},
    {C0, C0, -C0},
    {-C0, C0, C0},
    {-C0, -C0, -C0},
};
constexpr int FACES[4][3] = {
    {0, 1, 2},
    {1, 0, 3},
    {2, 3, 0},
    {3, 2, 1},
};

} // namespace tetrahedron
} // namespace polyhedra

#endif // POLYHEDRA_TETRAHEDRON_H
