.. _peirce_q:

********************************************************************************
Peirce Quincuncial
********************************************************************************

The Peirce Quincuncial projection is a conformal map projection
that transforms the circle of the northern hemisphere into a square,
and the southern hemisphere split into four triangles arranged
around the square to form a quincunx. The resulting projection
is a regular diamond shape or can be rotated to form a square.
The resulting tile can be infinitely tessellated. Though this implementation
defaults to a central meridian of 0, it is more common to use a central
meridian of around 25 to optimise the distortions. Peirce's original
published map from 1879 used a central meridian of approx -70.
The diamond and square versions can be produced using the
``+shape=diamond`` and ``+shape=square`` options respectively.
This implementation includes an alternative lateral projection
which places hemispheres side-by-side (``+shape=horizontal`` or
``+shape=vertical``). Combined with a general oblique transformation,
this can be used to produced a Grieger Triptychial projection
(see example below).

+---------------------+----------------------------------------------------------+
| **Classification**  | Miscellaneous                                            |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward spherical projection                             |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Alias**           | peirce_q                                                 |
+---------------------+----------------------------------------------------------+
| **Domain**          | 2D                                                       |
+---------------------+----------------------------------------------------------+
| **Input type**      | Geodetic coordinates                                     |
+---------------------+----------------------------------------------------------+
| **Output type**     | Projected coordinates                                    |
+---------------------+----------------------------------------------------------+


.. figure:: ./images/peirce_q_square.png
   :width: 500 px
   :align: center
   :alt:   Peirce Quincuncial (Square)

   proj-string: ``+proj=peirce_q +lon_0=25 +shape=square``

.. figure:: ./images/peirce_q_diamond.png
   :width: 500 px
   :align: center
   :alt:   Peirce Quincuncial (Diamond)

   proj-string: ``+proj=peirce_q +lon_0=25 +shape=diamond``

.. figure:: ./images/peirce_q_horizontal.png
   :width: 500 px
   :align: center
   :alt:   Peirce Quincuncial (Horizontal)

   proj-string: ``+proj=peirce_q +lon_0=25 +shape=horizontal``

.. figure:: ./images/grieger_triptychial.png
   :width: 500 px
   :align: center
   :alt:   Grieger Triptychial

   proj-string: ``+proj=pipeline +step +proj=ob_tran +o_proj=peirce_q +o_lat_p=-45 +o_lon_p=45 +o_type=horizontal +o_scrollx=-0.25 +step +proj=affine +s11=-1 +s12=0 +s21=0 +s22=-1``

Parameters
################################################################################

.. note:: All parameters are optional.

.. include:: ../options/lon_0.rst

.. option:: +shape=square/diamond/horizontal/vertical/nhemisphere/shemisphere

    .. versionadded:: 9.0

    *Defaults to diamond.*

    .. warning:: This option was wrongly introduced introduced in 8.2.1 with the
                 ``type`` name, which was inappropriate as it conflicted with
                 the ``+type=crs`` general hint.

    Indicates the shape of transformation applied to the southern hemisphere:
    ``square`` and ``diamond`` represent the traditional quincuncial form suggested
    by Peirce with the southern hemisphere divided into 4 triangles and reflected
    outward from the northern hemisphere. The ``square`` shape is rotated by 45
    degrees to produce the conventional square presentation. The origin lies at
    the centre of the square or diamond.

    By contrast, the ``horizontal`` and ``vertical`` forms reflect the southern
    hemisphere laterally across the x or y axis respectively to produce a rectangular
    form. The origin lies at the centre of the rectangle.

    The other two types, ``nhemisphere`` and ``shemisphere``, discard latitudes of less
    than 0 or more than 0, respectively, to allow single hemispheres to be selected.
    The origin lies at the centre of the square or diamond.

.. option:: +scrollx=<value>

    For ``horizontal`` shape allows a scalar circular scroll of resulting x coordinates
    to shift sections of the projection to the other horizontal side of the map.

    *Defaults to 0.0. Must be a scale between -1.0 and 1.0.*

.. option:: +scrolly=<value>

    For ``vertical`` shape allows a scalar circular scroll of resulting y coordinates
    to shift sections of the projection to the other vertical side of the map.

    *Defaults to 0.0. Must be a scale between -1.0 and 1.0.*

.. include:: ../options/R.rst

.. include:: ../options/x_0.rst

.. include:: ../options/y_0.rst
