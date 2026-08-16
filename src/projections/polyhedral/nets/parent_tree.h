/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Self-validating parent tree type
 * Author:   Felix Palmer
 *
 ****************************************************************************/

#ifndef NETS_PARENT_TREE_H
#define NETS_PARENT_TREE_H

namespace nets {

// Wrapper around int[NF] that ensures:
// - No entry is out of range [0, NF]
// - Exactly one entry is 0
template <int NF> struct ParentTree {
    int indices[NF];

    constexpr ParentTree(const int (&p)[NF]) : indices{} {
        int roots = 0;
        for (int i = 0; i < NF; i++) {
            if (p[i] < 0 || p[i] > NF)
                throw "parent tree entry must be in [0, NF]";
            if (p[i] == 0)
                roots++;
            indices[i] = p[i];
        }
        if (roots != 1)
            throw "parent tree must have exactly one root (a single 0 entry)";
    }

    // Allow direct access, as if using a normal array
    constexpr int operator[](int i) const { return indices[i]; }
};

} // namespace nets

#endif // NETS_PARENT_TREE_H
