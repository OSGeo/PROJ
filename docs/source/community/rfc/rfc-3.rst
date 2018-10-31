.. _rfc3:

====================================================================
PROJ RFC 3: Dependency management
====================================================================

:Author: Kristian Evers
:Contact: kreve@sdfe.dk
:Status: Draft
:Last Updated: 2018-10-31

Summary
-------------------------------------------------------------------------------

This document defines a rule set for dependency management in PROJ. With PROJ
being a core component in many downstream software packages clearly stating
which dependencies the library has is of great value. This document concern
both programming language standards as well as minimum required versions of
library dependencies and build tools.

It is propsed to adopt a rolling update scheme that ensures that PROJ is
sufficiently accessible, even on older systems, as well as keeping up with the
technological evolution. The scheme is divided in two parts, one concerning
versions of used programming languages within PROJ and the other concerning
software packages that PROJ depend on.

With adoption of this RFC, versions used for

1. programming languages will always be two iterations behind the most recent standard
2. software packages will always be at least two years old.

Following the above rule set will ensure that all but the most conservative
users of PROJ will be able to build and use the most recent version of the
library.

In the sections below details concerning programming languages and software
dependencies are outlined.

Background
-------------------------------------------------------------------------------

PROJ has traditionally been written in C89. Apart from that, no formal
requirements of e.g. build systems has been defined. In this RFC a rolling
update of version or standard requirements is described. The reasoning behind
a rolling update scheme is that it has become increasingly evident that C89
is becoming outdated and creating a less than optimal development environment
for contributors. It has been noted that the most commonly used compilers all
now support more recent versions of C, so the strict usage of C89 is no longer
as critical as it used to be.

Similarly, rolling updates to other tools and libraries that PROJ depend on
will ensure that the code base can be kept modern and in line with the rest of
the open source software ecosphere.

C and C++
-------------------------------------------------------------------------------

Following :ref:`RFC2 <rfc2>` PROJ is written in both C and C++. At the time of writing
the core library is C based and the code described in RFC2 is written in C++.
For the foreseeable future this will be upheld but it is likely that a shift
towards C++ will happen some time in the not-so-distant future.

Both the C and C++ standards are updated with regular intervals. After an
update of a standard it takes time for compiler manufacturers to implement the
standards fully, which makes adaption of new standards potentially troublesome
if done too soon. On the other hand, waiting too long to adopt new standards
will eventually make the code base feel old and new contributors are more likely
to stay away because they don't want to work using tools of the past. With a
rolling update scheme both concerns can be managed by always staying behind the
most recent standard, but not so far away that potential contributors are scared
away. Keeping a policy of always lagging behind be two iterations of the standard
is thought to be the best comprise between the two concerns.

C comes in four ISO standardised varieties: C89, C99, C11, C18. In this document
we refer to their informal names for ease of reading. C++ exists in five
varieties: C++98, C++03, C++11, C++14, C++17. Before adoption of this RFC PROJ
uses C89 and C++11. For C that means that the used standard is three iterations
behind the most recent standard. C++ is two iterations behind. After adoption of
this RFC the required standard for compilation of PROJ will always be two
iterations behind the most recent standard. This will result in a change of C
standard immediately after adoption of the RFC. When a new standard for either C
or C++ is released PROJ should change its requirement to the next standard in
the line. For C++ that means a change in standard roughly every three years, for
C the periods between standard updates is expected to be longer. Adaptation of
new programming language standards should be coordinated with a major version
release of PROJ. The initial switch to C99 will be coordinated with the release
of PROJ 6.0.0. Adaptation of C++14 is possible with PROJ 7.0.0, provided that
the C++20 standard is released in time.

Software dependencies
-------------------------------------------------------------------------------

At the time of writing PROJ is dependent on very few external packages.
In fact only one runtime dependency is present: SQLite. Building PROJ also
requires one of two external dependencies for configuration: Autotools or
CMake.

As with programming language standards it is preferable that software
dependencies are a bit behind the most recent development. For this reason it is
required that the minimum version supported in PROJ dependencies is at least two
years old, preferably more. It is not a requirement that the minimum
version of dependencies is always kept strictly two years behind current
development, but it is allowed in case future development of PROJ warrants an
update. Changes in minimum version requirements are allowed to happen with minor
version releases of PROJ.

At the time of writing the minimum version requried for SQLite it 3.7 which was
released in 2010. CMake currently is required to be at least at version 2.8.3
which was also released in 2010. For PROJ 7 a big overhaul of the CMake setup
is planned which will require CMake 3.5 which was released in 2016 and thus
meets the requirements of this RFC.

