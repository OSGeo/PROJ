.. _healpix:

********************************************************************************
HEALPix
********************************************************************************
+---------------------+----------------------------------------------------------+
| **Classification**  | Mixed                                                    |
+---------------------+----------------------------------------------------------+
| **Available forms** | Forward and inverse, spherical and elliptical projection |
+---------------------+----------------------------------------------------------+
| **Defined area**    | Global                                                   |
+---------------------+----------------------------------------------------------+
| **Implemented by**  | Alex Raichev and Michael Speth                           |
+---------------------+----------------------------------------------------------+
| **Options**                                                                    |
+---------------------+----------------------------------------------------------+
| `No special options for this projection`                                       |
+---------------------+----------------------------------------------------------+

.. image:: ../../images/healpix.png
   :scale: 75%
   :alt:   HEALPix

The HEALPix projection is area preserving and can be used with a spherical and
ellipsoidal model. It was initially developed for mapping cosmic background
microwave radiation. The image below is the graphical representation of the
mapping and consists of eight isomorphic triangular interrupted map graticules.
The north and south contains four in which straight meridians converge polewards
to a point and unequally spaced horizontal parallels. HEALPix provides a mapping
in which points of equal latitude and equally spaced longitude are mapped to points
of equal latitude and equally spaced longitude with the module of the polar
interruptions.


Usage
###############################################################################

To run a forward HEALPix projection on a unit sphere model, use the following command::

    proj +proj=healpix +lon_0=0 +a=1 -E <<EOF
    0 0
    EOF
    # output
    0 0 0.00 0.00

Further reading
################################################################################

#. `NASA <http://healpix.jpl.nasa.gov/>`_
#. `Wikipedia <https://en.wikipedia.org/wiki/HEALPix>`_





