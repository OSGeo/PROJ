.. _hgridshift:

================================================================================
Horizontal grid shift
================================================================================

Change of horizontal datum by grid shift.

+--------------+--------------------------------------------------------------------+
| **Options**                                                                       |
+--------------+--------------------------------------------------------------------+
| `+grids`     | Comma-separated list of grids to load.                             |
+--------------+--------------------------------------------------------------------+

The horizontal grid shift is done by offsetting the planar input coordinates by
a specific amount determined by the loaded grids. The simplest use case of the
horizontal grid shift is applying a single grid::

    +hgridshift +grids=nzgr2kgrid0005.gsb


More than one grid can be loaded at the same time, for instance in case the
dataset needs to be transformed spans several countries. In this example grids
of the continental US, Alaska and Canada is loaded at the same time::

    +hgridshift +grids=@conus,@alaska,@ntv2_0.gsb,@ntv_can.dat

The ``@`` in the above example states that the grid is optional, in case the grid
is not found in the PROJ.4 search path. The list of grids is prioritized so that
grids in the start of the list takes presedence over the grids in the back of the
list.

