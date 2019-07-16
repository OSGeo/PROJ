.. _rfc4:

====================================================================
PROJ RFC 4: PROJ Java Native Interface overhaul
====================================================================

:Author: Kristian Evers, Martin Desruisseaux
:Contact: kreve@sdfe.dk, martin.desruisseaux@geomatys.com
:Status: Draft
:Last Updated: 2019-07-16

Summary
-------------------------------------------------------------------------------

This RFC proposes a complete overhaul of the Java Native Interface (JNI) to PROJ.
In short, the current Java interface will be replaced by a new interface that
is based on GeoAPI and includes the new PROJ functionality described in RFC3.


Background
-------------------------------------------------------------------------------

The PROJ Java Native Interface exposes the ``proj_api.h`` API which was deprecated
with PROJ 6 and is scheduled for removal in PROJ 7. Java bindings has been
included in PROJ since version 4.4.9 (dated October 30, 2004). The JNI bindings
has not been given much attention for many years. They have been maintained
sporadically leaving the code in less than ideal shape. Currently the JNI
bindings target Java 8 which reached end of life in January 2019 (for the free
version).

With the above in mind, it is clear that action is needed. Either the Java
interface is removed altogether in PROJ 7 or it is updated to use modern tools
and based on the new ISO191111 compatible PROJ API. This RFC proposes the latter.
Updating the Java bindings is considered the best option because

1. PROJ has included Java bindings for 15 years, hence it is reasonable that
   users expect them to be there in the future as well
2. The Java interface will act as a demonstration of how to map another language
   to the PROJ API
3. The PROJ Java bindings will act as a reference implementation of GeoAPI 4
   which will expose PROJ to a larger audience and in turn increase the chances of
   contributions from the wider community

Unfortunately this also increases the maintenance burden on an area of PROJ
that historically has not been well-maintained and it adds several (optional) dependencies to the project. While this is not to be taken lightly, the authors
of this RFC assess that the benefits outweigh the drawbacks.


Solution
-------------------------------------------------------------------------------

This RFC details a complete replacement of the existing Java interface with a
new one based on the OGC standardised GeoAPI.

The proposed solution will target PROJ 7. A deprecation notice should be
published in the PROJ 6.2 release notes as well as a short outline of the
future state of the PROJ Java interface. This gives users of the current JNI
interface 6 months to prepare for the next version.

The updated Java interface to PROJ will be based on upcoming GeoAPI version 4.
We aim for a GeoAPI 4 release before the March 1st 2020 release of PROJ 7,
pending OGC approval. In case GeoAPI 4 does not exist in its final form at
that time the PROJ 7 release will feature a pre-release of GeoAPI 4 and the
release notes will warn users that they should expect minor updates in coming
PROJ versions.


API
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

The Java interface in PROJ 7 will be exposed as an implementation of `GeoAPI`_
as defined by `OGC`_. The JNI bindings will implement the upcomming version 4
of GeoAPI. GeoAPI 4 is based on ISO19111:2019, the same version of the standard
as the PROJ C/C++ API is based on. There will not be a one to one overlap of
the PROJ C API and the Java API but since they both implement the same
standard there will not be any compatibility issues.

The API will be accessible as a Java module  and package with the name
`org.osgeo.proj`.

.. _`GeoAPI`: https://www.opengeospatial.org/standards/geoapi
.. _`OGC`: http://www.opengeospatial.org/

Dependencies
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

The updated Java interface will be dependent on

  * Java 11 (run-time)
  * GeoAPI 4 (run-time)
  * JSR-363, Units of Measurement (run-time)
  * Maven (build)

Java 11 is chosen because it is a long time support release that will be
supported at least until late 2022.


Documentation
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Documentation of the Java interface will be done using JavaDoc. The JavaDoc
comments in the code will be used to generate static HTML files that document
the API, similarly to what is done with the PROJ C++ API and Doxygen. The
Travis continuous integration setup on GitHub will be extended to include
creation of the Java interface documentation. The generated HTML files will be
placed in their own directory which will be accessed through the URL
``https://proj.org/java/`` or something to that effect. At first the JavaDoc
documentation will not be integrated into the existing Sphinx documentation
that is otherwise used to document PROJ. Online documentation of Doxygen and
JavaDoc hints that it may be possible to integrate the JavaDoc documentation
in the same way as the C++ API is documented (via Doxygen and Breate). If time
allows, this will be investigated.

A tutorial on how to use the Java interface will be included in the Sphinx
docs for PROJ, so that it is readily accessible on the https://proj.org/.

Build instructions will be supplied in a README file located in the same
directory as the Java bindings itself. The build instructions will also be
included in the Sphinx docs as a section in the :ref:`Installation chapter <install>`.


Tests
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

The GIGS test suite will be implemented as JUnit 5 tests. The tests for the
new Java interface will be added as a target on the PROJ Continues
Integration setup.

Development
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

The development of the new Java interface will be done in a feature branch of a
clone of the PROJ repository. When the Java interface is starting to take form
it will be presented in a pull request on the PROJ GitHub repository for the
community to review the code as well as to confirm that the code integrates well
with the Continuous Integration setup.

The new code will be placed in a subfolder in the repository called ``java/``.
The existing ``jniwrap/`` folder will be removed.

The Java interface will only make use of the public API, either
using the C API available in ``proj..h`` and ``proj_experimental.h`` or the
C++ API found in ``include\proj``. No internal header files will be used. If
necessary, functions that are currently only available internally will be
lifted to the public API.

Adoption status
-------------------------------------------------------------------------------

The RFC is a draft currently in review.