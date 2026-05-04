/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Regular tetrahedron inscribed in the unit sphere.
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef POLYHEDRA_TETRAHEDRON_H
#define POLYHEDRA_TETRAHEDRON_H

#include "../conway.h"

namespace polyhedra {

// Following definition from https://dmccooey.com/polyhedra/Tetrahedron.txt
constexpr double C0 = 0.35355339059327376220042e+00; // sqrt(2) / 4

constexpr polyhedral::Mesh<4, 4, 3> tetrahedron = {
    // Vertices
    {
        {C0, -C0, C0},
        {C0, C0, -C0},
        {-C0, C0, C0},
        {-C0, -C0, -C0},
    },
    // Faces
    {
        {0, 1, 2},
        {1, 0, 3},
        {2, 3, 0},
        {3, 2, 1},
    },
};

} // namespace polyhedra

#endif // POLYHEDRA_TETRAHEDRON_H
