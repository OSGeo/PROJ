.. _transformation:

================================================================================
Ellipsoid  transformation
================================================================================

.. figure:: ./images/General_ellpsoid.png
   :width: 500 px
   :align: center
   :alt:   Adams Hemisphere in a Square

   proj-string: ````



A complete ellipsoid definition comprises a size (primary) and a shape (secondary)
parameter.

Ellipsoid Size parameters 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
.. _pj_ellipsoid :

    ============   ==============================================================
    Parameter      Description
    ============   ==============================================================
    +R             Radius of a spherical planet
    +a             Semimajor axis of an ellipsoidal planet
    ============   ==============================================================

Ellipsoid Shape parameters 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
.. _pj_ellipsoid :

    ============   ==============================================================
    Parameter      Description
    ============   ==============================================================
    +rf            Reverse flattening of the ellipsoid
    +f             Flattening of the ellipsoid
    +es            Eccentricity squared
    +e             Eccentricity
    +b             Semiminor axis
    ============   ==============================================================

The ellps=xxx parameter provides both size and shape for a number of built in
ellipsoid definitions.


The ellipsoid definition may be augmented with a spherification flag, turning
the ellipsoid into a sphere with features defined by the ellipsoid.

Ellipsoid Spherification parameters 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
.. _pj_ellipsoid :

    ============   ==============================================================
    Parameter      Description
    ============   ==============================================================
    +R_A            Gives a sphere with the same surface area and volume as the ellipsoid
    +R_a           which gives a sphere with R = (a + b)/2   (arithmetic mean)
    +R_g           gives a sphere with R = sqrt(a*b)   (geometric mean)
    +R_h            which gives a sphere with R = 2*a*b/(a+b) (harmonic mean)
    +R_lat_a=phi    which gives a sphere with R being the arithmetic mean of
                   of the corresponding ellipsoid at latitude phi.
    +R_lat_g=phi   which gives a sphere with R being the geometric mean of
                   of the corresponding ellipsoid at latitude phi.
    ============   ==============================================================
If R is given as size parameter, any shape and spherification parameters
given are ignored.

If size and shape are given as ellps=xxx, later shape and size parameters
are are taken into account as modifiers for the built in ellipsoid definition.

While this may seem strange, it is in accordance with historical PROJ
behavior. It can e.g. be used to define coordinates on the ellipsoid
scaled to unit semimajor axis by specifying "+ellps=xxx +a=1"

Transformation example
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Spherical earth with radius 7000km:

::proj=merc R=7000000

Using the GRS80 ellipsoid:

::proj=merc ellps=GRS80

Expressing ellipsoid by semi-major axis and reverse flattening (1/f):

::proj=merc a=6378137.0 rf=298.25

Spherical earth based on volume of ellipsoid

::proj=merc a=6378137.0 rf=298.25 +R_V