.. _unitconvert:

================================================================================
Unit conversion
================================================================================

Convert between various distance and time units.

+--------------+--------------------------------------------------------------------+
| **Options**                                                                       |
+--------------+--------------------------------------------------------------------+
| `+xy_in`     | Input unit of the horizontal components.                           |
+--------------+--------------------------------------------------------------------+
| `+xy_out`    | Output unit of the horizontal components.                          |
+--------------+--------------------------------------------------------------------+
| `+z_in`      | Input unit of the vertical component.                              |
+--------------+--------------------------------------------------------------------+
| `+z_out`     | Output unit of the vertical component.                             |
+--------------+--------------------------------------------------------------------+
| `+t_in`      | Input unit of the time component.                                  |
+--------------+--------------------------------------------------------------------+
| `+t_out`     | Output unit of the time component.                                 |
+--------------+--------------------------------------------------------------------+

There are many examples of coordinate reference systems that are expressed in
other units than the meter. There are also many cases where temporal data
has to be translated to different units. The `unitconvert` operation takes care
of that.

Many North American systems are defined with coordinates in feet. For example
in Vermont::

    +proj=pipeline
    +step +proj=tmerc +lat_0=42.5 +lon_0=-72.5 +k=0.999964286 +x_0=500000.00001016 +y_0=0
    +step +proj=unitconvert +xy_in=m +xy_out=us-ft

Often when working with GNSS data the timestamps are presented in GPS-weeks,
but when the data transformed with the `helmert` operation timestamps are
expected to be in units of decimalyears. This can be fixed with `unitconvert`::

    +proj=pipeline
    +step +proj=unitconvert +t_in=gpsweek +t_out=decimalyear
    +step +proj=helmert +epoch=2000.0 +t_obs=2017.5 ...


Distance units
###############################################################################

In the table below all distance units supported by PROJ is listed.
The same list can also be produced on the command line with `proj` or `cs2cs`,
by adding the `-lu` flag when calling the utility.

+----------+---------------------------------+
| Label    | Name                            |
+----------+---------------------------------+
|     km   | Kilometer                       |
+----------+---------------------------------+
|      m   | Meter                           |
+----------+---------------------------------+
|     dm   | Decimeter                       |
+----------+---------------------------------+
|     cm   | Centimeter                      |
+----------+---------------------------------+
|     mm   | Millimeter                      |
+----------+---------------------------------+
|    kmi   | International Nautical Mile     |
+----------+---------------------------------+
|     in   | International Inch              |
+----------+---------------------------------+
|     ft   | International Foot              |
+----------+---------------------------------+
|     yd   | International Yard              |
+----------+---------------------------------+
|     mi   | International Statute Mile      |
+----------+---------------------------------+
|   fath   | International Fathom            |
+----------+---------------------------------+
|     ch   | International Chain             |
+----------+---------------------------------+
|   link   | International Link              |
+----------+---------------------------------+
|  us-in   | U.S. Surveyor's Inch            |
+----------+---------------------------------+
|  us-ft   | U.S. Surveyor's Foot            |
+----------+---------------------------------+
|  us-yd   | U.S. Surveyor's Yard            |
+----------+---------------------------------+
|  us-ch   | U.S. Surveyor's Chain           |
+----------+---------------------------------+
|  us-mi   | U.S. Surveyor's Statute Mile    |
+----------+---------------------------------+
| ind-yd   | Indian Yard                     |
+----------+---------------------------------+
| ind-ft   | Indian Foot                     |
+----------+---------------------------------+
| ind-ch   | Indian Chain                    |
+----------+---------------------------------+


Time units
###############################################################################

In the table below all time units supported by PROJ is listed.

+--------------+-----------------------------+
| mjd          | Modified Julian date        |
+--------------+-----------------------------+
| decimalyear  | Decimal year                |
+--------------+-----------------------------+
| gps_week     | GPS Week                    |
+--------------+-----------------------------+

