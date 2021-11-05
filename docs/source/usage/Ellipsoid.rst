.. _transformation:

================================================================================
Ellipsoid  transformation
================================================================================
A ellipsoid is the mathematically defined surface which approximates the geiod, surface of the earth's gravity field, which is approximately the same as mean sea level.


.. figure:: ./images/General_ellpsoid.png
   :width: 500 px
   :align: center
   :alt:   Global and local fitting of the ellipsoid

   proj-string: ``+proj=merc``



A complete ellipsoid definition comprises a size (primary) and a shape (secondary)
parameter.

Ellipsoid Size parameters 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
.. _ellipsoid_parameters : PROJ/docs/source/operations/options/

	============   ==============================================================
	Parameter      Description
	============   ==============================================================
	+R             Radius of a spherical planet
	+a             Semi-major axis of an ellipsoidal planet
	============   ==============================================================

Ellipsoid Shape parameters 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
.. _ellipsoid_parameters : PROJ/docs/source/operations/options/

	============   ==============================================================
	Parameter      Description
	============   ==============================================================
	+rf            Reverse flattening of the ellipsoid
	+f             Flattening of the ellipsoid
	+es            Eccentricity squared
	+e             Eccentricity
	+b             Semi-minor axis
	============   ==============================================================

The ellipsoid definition may be augmented with a spherification flag, turning
the ellipsoid into a sphere with features defined by the ellipsoid.

Ellipsoid Spherification parameters 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
.. _ellipsoid_parameters : PROJ/docs/source/operations/options/

	============   ==============================================================
	Parameter      Description
	============   ==============================================================
	+R_A           A sphere with the same surface area and volume as the ellipsoid
	+R_a           A sphere with R = (a + b)/2   (arithmetic mean)
	+R_g           A sphere with R = sqrt(a*b)   (geometric mean)
	+R_h           A sphere with R = 2*a*b/(a+b) (harmonic mean)
	+R_lat_a=phi   A sphere with R being the arithmetic mean of the corresponding ellipsoid at latitude phi.
	+R_lat_g=phi   A sphere with R being the geometric mean of the corresponding ellipsoid at latitude phi.
	============   ==============================================================

If R is given as size parameter, any shape and spherification parameters
given are ignored.

The ellps=xxx parameter provides both size and shape for a number of built in
ellipsoid definitions.

Local Ellipsoid examples 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
.. _ellipsoid_parameters : PROJ/docs/source/operations/options/

	============   =================================    ============================            
	ellps          Parameters                           Datum name
	============   =================================    ============================                  
	=GRS80         a=6378137.0      rf=298.257222101    GRS 1980(IUGG, 1980)
	=airy          a=6377563.396    b=6356256.910       Airy 1830
	=bessel        a=6377397.155    rf=299.1528128      Bessel 1841
	=clrk66        a=6378206.4      b=6356583.8         Clarke 1866
	=intl          a=6378388.0      rf=297.             International 1909 (Hayford)
	=WGS60         a=6378165.0      rf=298.3            WGS 60
	=WGS66         a=6378145.0      rf=298.25           WGS 66
	=WGS72         a=6378135.0      rf=298.26           WGS 72
	=WGS84         a=6378137.0      rf=298.257223563    WGS 84
	=sphere        a=6370997.0      b=6370997.0         Normal Sphere (r=6370997)
	============   =================================    ============================

If size and shape are given as ellps=xxx, later shape and size parameters
are are taken into account as modifiers for the built in ellipsoid definition.

While this may seem strange, it is in accordance with historical PROJ
behavior. It can e.g. be used to define coordinates on the ellipsoid
scaled to unit semimajor axis by specifying "+ellps=xxx +a=1"

Transformation example
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Spherical earth with radius 7000km:

::
	proj=merc R=7000000

Using the GRS80 ellipsoid:

::
	proj=merc ellps=GRS80

Expressing ellipsoid by semi-major axis and reverse flattening (1/f):

::
	proj=merc a=6378137.0 rf=298.25

Spherical earth based on volume of ellipsoid

:: 
	proj=merc a=6378137.0 rf=298.25 +R_V

