.. _differences:

================================================================================
Known differences between versions
================================================================================

Once in a while, a new version of PROJ causes changes in the existing behaviour.
In this section we track deliberate changes to PROJ that break from previous
behaviour. Most times that will be caused by a bug fix. Unfortunately, some bugs
have existed for so long that their faulty behaviour is relied upon by software
that uses PROJ.

Behavioural changes caused by new bugs are not tracked here, as they should be
fixed in later versions of PROJ.

Version 5.0.0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Longitude wrapping when using custom central meridian
-------------------------------------------------------------------------------

By default PROJ wraps output longitudes in the range -180 to 180. Previous to
PROJ 5, this was handled incorrectly when a custom central meridian was set with
:option:`+lon_0`. This caused a change in sign on the resulting easting as seen
below::

    $ proj +proj=merc +lon_0=110
    -70 0
    -20037508.34    0.00
    290 0
    20037508.34     0.00

From PROJ 5 on onwards, the same input now results in same coordinates, as seen
from the example below where PROJ 5 is used::

    $ proj +proj=merc +lon_0=110
    -70 0
    -20037508.34    0.00
    290 0
    -20037508.34    0.00

The change is made on the basis that :math:`\lambda=290^{\circ}` is a full
rotation of the circle larger than :math:`\lambda=-70^{\circ}` and hence
should return the same output for both.

Adding the ``+over`` flag to the projection definition provides
the old behaviour.
