.. _ellipsoids:

================================================================================
Ellipsoids
================================================================================
An ellipsoid is a mathematically defined surface which approximates the *geoid*:
the surface of the Earth's gravity field, which is approximately the same as
mean sea level.


.. figure:: ../../images/general_ellipsoid.png
   :width: 500 px
   :align: center
   :alt:   Global and local fitting of the ellipsoid

   Global and local fitting of the ellipsoid


A complete ellipsoid definition comprises a size (primary) and a shape (secondary)
parameter.

.. _ellipsoid_size_parameters:

Ellipsoid size parameters
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. option:: +R=<value>

   Radius of the sphere, :math:`R`.

.. option:: +a=<value>

   Semi-major axis of the ellipsoid, :math:`a`.

.. _ellipsoid_shape_parameters:

Ellipsoid shape parameters
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. option:: +rf=<value>

   Reverse flattening of the ellipsoid, :math:`1/f`.

.. option:: +f=<value>

   Flattening of the ellipsoid, :math:`f`.

.. option:: +es=<value>

   Eccentricity squared, :math:`e^2`.

.. option:: +e=<value>

   Eccentricity, :math:`e`.

.. option:: +b=<value>

   Semi-minor axis, :math:`b`.

The ellipsoid definition may be augmented with a spherification flag, turning
the ellipsoid into a sphere with features defined by the ellipsoid.

.. _ellipsoid_spherification_parameters:

Ellipsoid spherification parameters
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.. option:: +R_A=<value>

   A sphere with the same surface area as the ellipsoid.

.. option:: +R_V=<value>

   A sphere with the same volume as the ellipsoid.

.. option:: +R_a=<value>

   A sphere with :math:`R = (a + b)/2` (arithmetic mean).

.. option:: +R_g=<value>

   A sphere with :math:`R = \sqrt{ab}` (geometric mean).

.. option:: +R_h=<value>

   A sphere with :math:`R = 2ab/(a+b)` (harmonic mean).

.. option:: +R_lat_a=<phi>

   A sphere with :math:`R` being the arithmetic mean of the corresponding
   ellipsoid at latitude :math:`\phi`.

.. option:: +R_lat_g=<phi>

   A sphere with :math:`R` being the geometric mean of the corresponding
   ellipsoid at latitude :math:`\phi`.

If :option:`+R` is given as size parameter, any shape and spherification
parameters given are ignored.

.. _ellipsoid_definitions:

Built-in ellipsoid definitions
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

The ``ellps=xxx`` parameter provides both size and shape for a number of
built-in ellipsoid definitions.

    ============   =================================    ============================
    ellps          Parameters                           Datum name
    ============   =================================    ============================
    GRS80          a=6378137.0      rf=298.257222101    GRS 1980(IUGG, 1980)
    airy           a=6377563.396    b=6356256.910       Airy 1830
    bessel         a=6377397.155    rf=299.1528128      Bessel 1841
    clrk66         a=6378206.4      b=6356583.8         Clarke 1866
    intl           a=6378388.0      rf=297.             International 1909 (Hayford)
    WGS60          a=6378165.0      rf=298.3            WGS 60
    WGS66          a=6378145.0      rf=298.25           WGS 66
    WGS72          a=6378135.0      rf=298.26           WGS 72
    WGS84          a=6378137.0      rf=298.257223563    WGS 84
    sphere         a=6370997.0      b=6370997.0         Normal Sphere (r=6370997)
    ============   =================================    ============================

If size and shape are given as ``ellps=xxx``, later shape and size parameters
are are taken into account as modifiers for the built-in ellipsoid definition.

While this may seem strange, it is in accordance with historical PROJ
behavior. It can e.g. be used to define coordinates on the ellipsoid
scaled to unit semimajor axis by specifying ``+ellps=xxx +a=1``

.. _ellipsoid_transformation_examples:

Transformation examples
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Spherical earth with radius 7000km:

::

    +proj=latlon +R=7000000

Using the GRS80 ellipsoid:

::

    +proj=latlon +ellps=GRS80

Expressing ellipsoid by semi-major axis and reverse flattening (:math:`1/f`):

::

    +proj=latlon +a=6378137.0 +rf=298.25

Spherical earth based on volume of ellipsoid

::

    +proj=latlon +a=6378137.0 +rf=298.25 +R_V

