#!/usr/bin/env python
###############################################################################
# $Id$
#
#  Project:  PROJ
#  Purpose:  Build airocean parameters
#  Author:   Pierre Louvart <pierre.louvart at ifremer.fr>
#
###############################################################################
#  Copyright (c) 2025, Pierre Louvart <pierre.louvart at ifremer.fr>
#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included
#  in all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
#  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
###############################################################################

"""
    This python script can be launched "as is" with no additional argument.
    It will generate 2 plots (require geopandas and matplotlib):
    - one for the 20 faces unfolded net
    - one for the complete 23 faces unfolded net that makes up the final airocean map
    It will also print on the standard output the C code initializing vertices, faces and
    transform matrices (forward and inverse) required to conduct the airocean projection.
"""


import numpy as np
from pathlib import Path
import json

np.set_printoptions(precision=22)

"""
    Zap values close to 1s and 0s
"""


def zap_zero_or_one(vals, eps=1e-12, verbose=False):
    """Return copy of array with values very close to zero and one set exactly."""
    vals = vals.copy()
    sel = (np.abs(vals) < eps) & (vals != 0.0)
    if sel.any():
        vals[sel] = 0.0
        if verbose:
            print(f"adjusted {sel.sum()} zeros")
    ones = np.abs(np.abs(vals) - 1.0)
    sel = (ones < eps) & (ones != 0.0)
    if sel.any():
        vals[sel] = np.sign(vals[sel])
        if verbose:
            print(f"adjusted {sel.sum()} ones")
    return vals


"""
    Definition of the vertices of the unfolded icosahedron net:
"""


# Icosahedron properties
circumscribed_radius = 1
inscribed_radius = 3**0.5 / 12 * (3 + 5**0.5)
triangle_edge = float(circumscribed_radius / np.sin(2 * np.pi / 5))
triangle_height = 3**0.5 / 2 * triangle_edge

# Icosahedron properties scaled to a specific radius. Left to 1.0
radius = 1.0
triangle_earth_edge = triangle_edge * radius
triangle_earth_height = triangle_height * radius
circumscribed_earth_radius = circumscribed_radius * radius
inscribed_earth_radius = inscribed_radius * radius


# Vertices of the faces in the airocean referential
airocean_vertices = np.array(
    [
        [2, 0, 0],  # Vertex 0
        [1, 1, 0],  # Vertex 1
        [3, 1, 0],  # Vertex 2
        [0, 2, 0],  # Vertex 3
        [2, 2, 0],  # Vertex 4
        [1, 3, 0],  # Vertex 5
        [3, 3, 0],  # Vertex 6
        [0, 4, 0],  # Vertex 7
        [2, 4, 0],  # Vertex 8
        [1, 5, 0],  # Vertex 9
        [3, 5, 0],  # Vertex 10
        [0, 6, 0],  # Vertex 11
        [2, 6, 0],  # Vertex 12
        [1, 7, 0],  # Vertex 13
        [3, 7, 0],  # Vertex 14
        [0, 8, 0],  # Vertex 15
        [2, 8, 0],  # Vertex 16
        [1, 9, 0],  # Vertex 17
        [3, 9, 0],  # Vertex 18
        [0, 10, 0],  # Vertex 19
        [2, 10, 0],  # Vertex 20
        [1, 11, 0],  # Vertex 21
        [0.5, 9.5, 0],  # Vertex 22 : Split from Face 15 (ocean)
        [1, 0, 0],  # Vertex 23 : Split from Face 15 (australia)
        [2 / 3, 8, 0],  # Vertex 24 : split from face 8 (japan)
        [1 / 3, 7, 0],  # Vertex 25 : new from face 8 (japan)
        [1, -1, 0],  # Vertex 26 : bottom new for antartica
    ]
) * [triangle_earth_height, triangle_earth_edge / 2, 1]


"""
    Definition of the airocean "base" (read non-split) faces and centers
"""

airocean_face_vertices = airocean_vertices[
    [
        [12, 16, 14],  # Face 0
        [12, 13, 16],  # Face 1
        [12, 9, 13],  # Face 2
        [12, 8, 9],  # Face 3
        [12, 14, 10],  # Face 4
        [14, 16, 18],  # Face 5
        [20, 16, 17],  # Face 6
        [17, 16, 13],  # Face 7
        [9, 11, 13],  # Face 9
        [9, 5, 7],  # Face 10
        [9, 8, 5],  # Face 11
        [5, 8, 4],  # Face 12
        [4, 8, 6],  # Face 13
        [0, 4, 2],  # Face 14
        [19, 17, 15],  # Face 16
        [3, 7, 5],  # Face 17
        [1, 5, 4],  # Face 18
        [1, 4, 0],  # Face 19
        [15, 17, 13],  # Face 8
        [19, 21, 17],  # Face 15
        [26, 1, 0],  # Face 8'
        [11, 15, 13],  # Face 15'
    ]
]
airocean_face_vertices = np.concatenate(
    [
        airocean_face_vertices,
        np.zeros((*airocean_face_vertices.shape[:-1], 1)),
    ],
    axis=2,
)

airocean_22_centers = airocean_face_vertices.mean(axis=1)
airocean_22_centers[:, 2] = 1
airocean_2_centers = airocean_22_centers[[18, 19]]


"""
    Definition of the complete faces of the
    airocean unfolded net (23 faces total).
"""

airocean_23_face_vertices = airocean_vertices[
    [
        [12, 16, 14],  # Face 0
        [12, 13, 16],  # Face 1
        [12, 9, 13],  # Face 2
        [12, 8, 9],  # Face 3
        [12, 14, 10],  # Face 4
        [14, 16, 18],  # Face 5
        [20, 16, 17],  # Face 6
        [17, 16, 13],  # Face 7
        # [15, 17, 13],  # Face 8, split
        [9, 11, 13],  # Face 9
        [9, 5, 7],  # Face 10
        [9, 8, 5],  # Face 11
        [5, 8, 4],  # Face 12
        [4, 8, 6],  # Face 13
        [0, 4, 2],  # Face 14
        # [19, 21, 17],  # Face 15, split
        [19, 17, 15],  # Face 16
        [3, 7, 5],  # Face 17
        [1, 5, 4],  # Face 18
        [1, 4, 0],  # Face 19
        [17, 22, 21],  # Face 20 - from 15 (Ocean)
        [1, 0, 23],  # Face 21 - from 15 (Australia)
        [15, 17, 24],  # Face 22 - from 8 (Japan)
        [17, 13, 24],  # Face 23 - from 8 (Japan)
        [11, 25, 13],  # Face 24 - from 8 (Japan)
    ]
]
airocean_23_face_vertices = np.concatenate(
    [
        airocean_23_face_vertices,
        np.ones((*airocean_23_face_vertices.shape[:-1], 1)),
    ],
    axis=2,
)
airocean_23_face_vertices[:, :, 3] = 1.0


airocean_23_centers = airocean_23_face_vertices.mean(axis=1)
airocean_23_centers[:, 2] = 1


"""
    Definition of the forward and inverse transform matrices to switch
    from vertical to horizontal layout
"""


def translation(dx=0, dy=0):
    res = np.eye(4, dtype=np.float64)
    res[[0, 1], 3] = dx, dy
    return res


def rotation(angle):
    res = np.eye(4)
    res[:2, :2] = zap_zero_or_one(
        np.array(
            [
                [np.cos(angle), -np.sin(angle)],
                [np.sin(angle), np.cos(angle)],
            ],
            dtype=np.float64,
        )
    )
    return res


w, h = airocean_vertices[:, 0].max(), airocean_vertices[:, 1].max()
orient_horizontal = zap_zero_or_one(
    translation(h, 0) @ rotation(np.pi / 2 * 1)
)
inv_orient_horizontal = zap_zero_or_one(np.linalg.inv(orient_horizontal))


"""
    Definition of icosahedron vertices, faces and centers.
    Credit to Gray Fuller for the original values.
    Original source: http://www.rwgrayprojects.com/rbfnotes/maps/docs/graypr.c
"""

ico_faces = np.array(
    [
        [
            [
                0.420152426708710003,
                0.078145249402782959,
                0.904082550615019298,
                1.0,
            ],
            [
                0.518836730327364437,
                0.835420380378235850,
                0.181331837557262454,
                1.0,
            ],
            [
                0.995009439436241649,
                -0.091347795276427931,
                0.040147175877166645,
                1.0,
            ],
        ],
        [
            [
                0.420152426708710003,
                0.078145249402782959,
                0.904082550615019298,
                1.0,
            ],
            [
                -0.414682225320335218,
                0.655962405434800777,
                0.630675807891475371,
                1.0,
            ],
            [
                0.518836730327364437,
                0.835420380378235850,
                0.181331837557262454,
                1.0,
            ],
        ],
        [
            [
                0.420152426708710003,
                0.078145249402782959,
                0.904082550615019298,
                1.0,
            ],
            [
                -0.515455959944041808,
                -0.381716898287133011,
                0.767200992517747538,
                1.0,
            ],
            [
                -0.414682225320335218,
                0.655962405434800777,
                0.630675807891475371,
                1.0,
            ],
        ],
        [
            [
                0.420152426708710003,
                0.078145249402782959,
                0.904082550615019298,
                1.0,
            ],
            [
                0.355781402532944713,
                -0.843580002466178147,
                0.402234226602925571,
                1.0,
            ],
            [
                -0.515455959944041808,
                -0.381716898287133011,
                0.767200992517747538,
                1.0,
            ],
        ],
        [
            [
                0.420152426708710003,
                0.078145249402782959,
                0.904082550615019298,
                1.0,
            ],
            [
                0.995009439436241649,
                -0.091347795276427931,
                0.040147175877166645,
                1.0,
            ],
            [
                0.355781402532944713,
                -0.843580002466178147,
                0.402234226602925571,
                1.0,
            ],
        ],
        [
            [
                0.995009439436241649,
                -0.091347795276427931,
                0.040147175877166645,
                1.0,
            ],
            [
                0.518836730327364437,
                0.835420380378235850,
                0.181331837557262454,
                1.0,
            ],
            [
                0.515455959944041808,
                0.381716898287133011,
                -0.767200992517747538,
                1.0,
            ],
        ],
        [
            [
                0.515455959944041808,
                0.381716898287133011,
                -0.767200992517747538,
                1.0,
            ],
            [
                0.518836730327364437,
                0.835420380378235850,
                0.181331837557262454,
                1.0,
            ],
            [
                -0.355781402532944713,
                0.843580002466178147,
                -0.402234226602925571,
                1.0,
            ],
        ],
        [
            [
                -0.355781402532944713,
                0.843580002466178147,
                -0.402234226602925571,
                1.0,
            ],
            [
                0.518836730327364437,
                0.835420380378235850,
                0.181331837557262454,
                1.0,
            ],
            [
                -0.414682225320335218,
                0.655962405434800777,
                0.630675807891475371,
                1.0,
            ],
        ],
        [
            [
                -0.515455959944041808,
                -0.381716898287133011,
                0.767200992517747538,
                1.0,
            ],
            [
                -0.995009439436241649,
                0.091347795276427931,
                -0.040147175877166645,
                1.0,
            ],
            [
                -0.414682225320335218,
                0.655962405434800777,
                0.630675807891475371,
                1.0,
            ],
        ],
        [
            [
                -0.515455959944041808,
                -0.381716898287133011,
                0.767200992517747538,
                1.0,
            ],
            [
                -0.518836730327364437,
                -0.835420380378235850,
                -0.181331837557262454,
                1.0,
            ],
            [
                -0.995009439436241649,
                0.091347795276427931,
                -0.040147175877166645,
                1.0,
            ],
        ],
        [
            [
                -0.515455959944041808,
                -0.381716898287133011,
                0.767200992517747538,
                1.0,
            ],
            [
                0.355781402532944713,
                -0.843580002466178147,
                0.402234226602925571,
                1.0,
            ],
            [
                -0.518836730327364437,
                -0.835420380378235850,
                -0.181331837557262454,
                1.0,
            ],
        ],
        [
            [
                -0.518836730327364437,
                -0.835420380378235850,
                -0.181331837557262454,
                1.0,
            ],
            [
                0.355781402532944713,
                -0.843580002466178147,
                0.402234226602925571,
                1.0,
            ],
            [
                0.414682225320335218,
                -0.655962405434800777,
                -0.630675807891475371,
                1.0,
            ],
        ],
        [
            [
                0.414682225320335218,
                -0.655962405434800777,
                -0.630675807891475371,
                1.0,
            ],
            [
                0.355781402532944713,
                -0.843580002466178147,
                0.402234226602925571,
                1.0,
            ],
            [
                0.995009439436241649,
                -0.091347795276427931,
                0.040147175877166645,
                1.0,
            ],
        ],
        [
            [
                0.515455959944041808,
                0.381716898287133011,
                -0.767200992517747538,
                1.0,
            ],
            [
                0.414682225320335218,
                -0.655962405434800777,
                -0.630675807891475371,
                1.0,
            ],
            [
                0.995009439436241649,
                -0.091347795276427931,
                0.040147175877166645,
                1.0,
            ],
        ],
        [
            [
                -0.420152426708710003,
                -0.078145249402782959,
                -0.904082550615019298,
                1.0,
            ],
            [
                -0.355781402532944713,
                0.843580002466178147,
                -0.402234226602925571,
                1.0,
            ],
            [
                -0.995009439436241649,
                0.091347795276427931,
                -0.040147175877166645,
                1.0,
            ],
        ],
        [
            [
                -0.420152426708710003,
                -0.078145249402782959,
                -0.904082550615019298,
                1.0,
            ],
            [
                -0.995009439436241649,
                0.091347795276427931,
                -0.040147175877166645,
                1.0,
            ],
            [
                -0.518836730327364437,
                -0.835420380378235850,
                -0.181331837557262454,
                1.0,
            ],
        ],
        [
            [
                -0.420152426708710003,
                -0.078145249402782959,
                -0.904082550615019298,
                1.0,
            ],
            [
                -0.518836730327364437,
                -0.835420380378235850,
                -0.181331837557262454,
                1.0,
            ],
            [
                0.414682225320335218,
                -0.655962405434800777,
                -0.630675807891475371,
                1.0,
            ],
        ],
        [
            [
                -0.420152426708710003,
                -0.078145249402782959,
                -0.904082550615019298,
                1.0,
            ],
            [
                0.414682225320335218,
                -0.655962405434800777,
                -0.630675807891475371,
                1.0,
            ],
            [
                0.515455959944041808,
                0.381716898287133011,
                -0.767200992517747538,
                1.0,
            ],
        ],
        [
            [
                -0.355781402532944713,
                0.843580002466178147,
                -0.402234226602925571,
                1.0,
            ],
            [
                -0.38796691462082733,
                0.3827173765316976,
                -0.6531583886089725,
                1.0,
            ],
            [
                0.515455959944041808,
                0.381716898287133011,
                -0.767200992517747538,
                1.0,
            ],
        ],
        [
            [
                -0.420152426708710003,
                -0.078145249402782959,
                -0.904082550615019298,
                1.0,
            ],
            [
                0.515455959944041808,
                0.381716898287133011,
                -0.767200992517747538,
                1.0,
            ],
            [
                -0.38796691462082733,
                0.3827173765316976,
                -0.6531583886089725,
                1.0,
            ],
        ],
        [
            [
                -0.995009439436241649,
                0.091347795276427931,
                -0.040147175877166645,
                1.0,
            ],
            [
                -0.355781402532944713,
                0.843580002466178147,
                -0.402234226602925571,
                1.0,
            ],
            [
                -0.5884910224298405,
                0.5302967343924689,
                0.06276480180379439,
                1.0,
            ],
        ],
        [
            [
                -0.355781402532944713,
                0.843580002466178147,
                -0.402234226602925571,
                1.0,
            ],
            [
                -0.414682225320335218,
                0.655962405434800777,
                0.630675807891475371,
                1.0,
            ],
            [
                -0.5884910224298405,
                0.5302967343924689,
                0.06276480180379439,
                1.0,
            ],
        ],
        [
            [
                -0.995009439436241649,
                0.091347795276427931,
                -0.040147175877166645,
                1.0,
            ],
            [
                -0.5884910224298405,
                0.5302967343924689,
                0.06276480180379439,
                1.0,
            ],
            [
                -0.414682225320335218,
                0.655962405434800777,
                0.630675807891475371,
                1.0,
            ],
        ],
    ],
    dtype=np.float64,
)
ico_20_centers = np.array(
    [
        [0.6446661988241054, 0.27407261150153034, 0.37518718801648276],
        [0.17476897723857973, 0.5231760117386065, 0.5720300653545857],
        [-0.16999525285188902, 0.1174635855168169, 0.7673197836747474],
        [0.08682595643253761, -0.3823838837835094, 0.6911725899118975],
        [0.5903144228926321, -0.28559418277994103, 0.44882131769837047],
        [0.6764340432358825, 0.375263161129647, -0.18190732636110615],
        [0.22617042924615385, 0.6869057603771823, -0.3293677938544702],
        [-0.0838756325086385, 0.778320929426405, 0.13659113961527075],
        [-0.6417158749002062, 0.12186443414136523, 0.45257654151068544],
        [-0.6764340432358825, -0.375263161129647, 0.18190732636110615],
        [-0.22617042924615385, -0.6869057603771823, 0.32936779385447024],
        [0.0838756325086385, -0.778320929426405, -0.13659113961527075],
        [0.5884910224298405, -0.5302967343924689, -0.06276480180379439],
        [0.6417158749002062, -0.12186443414136523, -0.4525765415106855],
        [-0.5903144228926321, 0.28559418277994103, -0.44882131769837047],
        [-0.6446661988241054, -0.2740726115015303, -0.37518718801648276],
        [-0.17476897723857973, -0.5231760117386065, -0.5720300653545857],
        [0.16999525285188902, -0.11746358551681692, -0.7673197836747474],
        [-0.5884910224298405, 0.5302967343924689, 0.06276480180379439],
        [-0.08682595643253764, 0.3823838837835094, -0.6911725899118975],
    ],
    dtype=np.float64,
)


"""
    Definition of the forward and inverse transform matrices from icosahedral faces to airocean unfolded net space
"""

A = ico_faces[:, 1] - ico_faces[:, 0]
B = ico_faces[:, 2] - ico_faces[:, 0]
ico_normals = -np.array(
    [
        A[:, 1] * B[:, 2] - A[:, 2] * B[:, 1],
        A[:, 2] * B[:, 0] - A[:, 0] * B[:, 2],
        A[:, 0] * B[:, 1] - A[:, 1] * B[:, 0],
    ]
).T
ico_normals /= ((ico_normals**2).sum(axis=1) ** 0.5).reshape(-1, 1)
ico_centers = ico_faces.mean(axis=1)
ico_elevated_centers = ico_centers.copy()
ico_elevated_centers[:, :3] += ico_normals

airocean_basis = np.zeros((23, 4, 4))
airocean_basis[:, :3, :] = airocean_23_face_vertices[:, :3, :]
airocean_basis[:, 3, :] = airocean_23_centers

ico_basis = np.zeros((23, 4, 4))
ico_basis[:, :3, :] = ico_faces
ico_basis[:, 3, :] = ico_elevated_centers


air_ico_trans = zap_zero_or_one(
    np.array(
        [
            a.T @ np.linalg.inv(b.T)
            for i, (a, b) in enumerate(zip(ico_basis, airocean_basis))
        ],
        dtype=np.float64,
    )
)
ico_air_trans = zap_zero_or_one(
    np.array(
        [
            b.T @ np.linalg.inv(a.T)
            for i, (a, b) in enumerate(zip(ico_basis, airocean_basis))
        ],
        dtype=np.float64,
    )
)


for i, (a, b, v1, v2) in enumerate(
    zip(ico_basis, airocean_basis, ico_centers, airocean_22_centers)
):
    m = b.T @ np.linalg.inv(a.T)  # ico to air
    w = a.T @ np.linalg.inv(b.T)  # air to ico
    for x, y in zip(a, b):
        v1 = m @ x.T
        v1 /= v1[3]
        v2 = w @ y.T
        v2 /= v2[3]
        assert np.isclose(v1, y, rtol=1e-7).all()
        assert np.isclose(v2, x, rtol=1e-7).all()

    # Assert that the transform matrices work adequately
    for i in range(100):
        r = np.random.rand(3)
        r /= r.sum()
        v = (a[:3, :] * r.reshape(3, 1)).sum(axis=0)
        v1 = m @ v.T
        v1 /= v1[3]
        v2 = w @ v1.T
        v2 /= v2[3]
        assert np.isclose(v, v2, rtol=1e-7).all()


"""
    End of definitions.
"""


def generate_20_faces_airocean_net(
    outfile: Path,
) -> None:
    """
    Save a picture of the "base" (non split) 20 faces of the unfolded icosahedron net
    """
    try:
        import geopandas as gpd
        from shapely.geometry import Polygon

        gdf = gpd.GeoDataFrame(
            data={"id": range(22)},
            geometry=[Polygon(shell=v[:, :2]) for v in airocean_face_vertices],
            crs=None,
        )
        ax = gdf.plot()
        gdf.apply(
            lambda x: ax.annotate(
                text=x["id"], xy=x.geometry.centroid.coords[0], ha="center"
            ),
            axis=1,
        )
        ax.get_figure().savefig(outfile)
    except ImportError as e:
        print(
            f"Geopandas and shapely must be installed to generate airocean plot! ({e})"
        )


def generate_23_faces_airocean_net(
    outfile: Path,
) -> None:
    """
    Save a picture of the complete 23 faces of the unfolded icosahedron net
    """
    try:
        import geopandas as gpd
        from shapely.geometry import Polygon

        gdf = gpd.GeoDataFrame(
            data={"id": range(len(airocean_23_face_vertices))},
            geometry=[
                Polygon(shell=v[:, :2]) for v in airocean_23_face_vertices
            ],
            crs=None,
        )
        ax = gdf.plot()
        gdf.apply(
            lambda x: ax.annotate(
                text=x["id"], xy=x.geometry.centroid.coords[0], ha="center"
            ),
            axis=1,
        )
        ax.get_figure().savefig(outfile)
    except ImportError as e:
        print(
            f"Geopandas and shapely must be installed to generate airocean plot! ({e})"
        )


def generate_airocean_parameters() -> str:
    """
    Create a string of C code that initializes airocean parameters
    and that can be included directly into the source file of the airocean projection.
    """
    return "\n".join(
        [
            (
                f"{name} = "
                + json.dumps(value.tolist())
                .replace("[", "{")
                .replace("]", "}")
                + ";"
            )
            for name, value in {
                "constexpr pj_face base_ico_faces[23]": ico_faces[:, :, :3],
                "constexpr PJ_XYZ base_ico_centers[23]": ico_centers[:, :3],
                "constexpr PJ_XYZ base_ico_normals[23]": ico_normals,
                "constexpr pj_face base_airocean_faces[23]": airocean_23_face_vertices[
                    :, :, [0, 1, 3]
                ],
                "constexpr double base_ico_air_trans[23][4][4]": ico_air_trans,
                "constexpr double base_air_ico_trans[23][4][4]": air_ico_trans,
                "constexpr double orient_horizontal_trans[4][4]": orient_horizontal,
                "constexpr double orient_horizontal_inv_trans[4][4]": inv_orient_horizontal,
            }.items()
        ]
    )


if __name__ == "__main__":
    generate_20_faces_airocean_net(Path("./20_faces_airocean_net.png"))
    generate_23_faces_airocean_net(Path("./23_faces_airocean_net.png"))
    parameters = generate_airocean_parameters()
    print(parameters)
