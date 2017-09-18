.. _vgridshift:

================================================================================
Vertical grid shift
================================================================================

Change Vertical datum change by grid shift

+--------------+--------------------------------------------------------------------+
| **Options**                                                                       |
+--------------+--------------------------------------------------------------------+
| `+grids`     | Comma-separated list of grids to load.                             |
+--------------+--------------------------------------------------------------------+

The vertical grid shift is done by offsetting the vertical input coordinates by
a specific amount determined by the loaded grids. The simplest use case of the
horizontal grid shift is applying a single grid. Here we change the vertical
reference from the ellipsoid to the global geoid model, EGM96::

    +vgridshift +grids=egm96_16.gtx


More than one grid can be loaded at the same time, for instance in the case where
a better geoid model than the global is available for a certain area. Here the
gridshift is set up so that the local DVR90 geoid model takes presedence over
the global model::

    +vgridshift +grids=@dvr90.gtx,egm96_16.gtx

The ``@`` in the above example states that the grid is optional, in case the grid
is not found in the PROJ.4 search path. The list of grids is prioritized so that
grids in the start of the list takes presedence over the grids in the back of the
list.

