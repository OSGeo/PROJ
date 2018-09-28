###############################################################################
About
###############################################################################

PROJ is a generic coordinate transformation software, that transforms geospatial
coordinates from one coordinate reference system (CRS) to another. This
includes cartographic projections as well as geodetic transformations.

PROJ includes :ref:`command line applications<apps>` for easy conversion of
coordinates from text files or directly from user input. In addition to the
command line utilities PROJ also exposes an
:ref:`application programming interface<reference>`, or API in short. The API
let developers use the functionality of PROJ in their own software without having
to implement similar functionality themselves.

PROJ started purely as a cartography application letting users convert geodetic
coordinates into projected coordinates using a number of different cartographic
projections. Over the years, as the need has become apparent, support for datum
shifts has slowly worked it's way into PROJ as well. Today PROJ support more
than a hundred different map projections and can transform coordinates between
datums using all but the most obscure geodetic techniques.

Citation
-------------------------------------------------------------------------------

To cite PROJ in publications use:

  PROJ contributors (2018). PROJ coordinate transformation software
  library. Open Source Geospatial Foundation. URL https://proj4.org/.

A BibTeX entry for LaTeX users is

.. code-block:: latex

  @Manual{,
    title = {{PROJ} coordinate transformation software library},
    author = {{PROJ contributors}},
    organization = {Open Source Geospatial Foundation},
    year = {2018},
    url = {https://proj4.org/},
  }

License
-------------------------------------------------------------------------------

PROJ uses the MIT license. The software was initially released by the USGS in
the public domain. When Frank Warmerdam took over the development of PROJ it
was moved under the MIT license. The license is as follows:

     Copyright (c) 2000, Frank Warmerdam

     Permission is hereby granted, free of charge, to any person obtaining a
     copy of this software and associated documentation files (the "Software"),
     to deal in the Software without restriction, including without limitation
     the rights to use, copy, modify, merge, publish, distribute, sublicense,
     and/or sell copies of the Software, and to permit persons to whom the
     Software is furnished to do so, subject to the following conditions:

     The above copyright notice and this permission notice shall be included
     in all copies or substantial portions of the Software.

     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
     OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
     FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
     THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
     LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
     FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
     DEALINGS IN THE SOFTWARE.

