.. _rfc7:

====================================================================
PROJ RFC 7: Drop Autotools, maintain CMake
====================================================================

:Author: Mike Taves
:Contact: mwtoews@gmail.com
:Status: Adopted
:Implementation target: PROJ 9.0
:Last Updated: 2021-10-27

Summary
-------------------------------------------------------------------------------

This RFC proposes to drop Autotools for PROJ 9.0, and to maintain CMake
for build automation, testing and packaging. This will reduce the overall
maintenance for PROJ and will enable the library to be integrated into other
projects that use CMake.

Background
-------------------------------------------------------------------------------

Here is a short timeline of the build tools used for PROJ:

- Throughout the mid-1990s, Gerald Evenden maintained a Unix build system with
  a few scripts (some derived from Autoconf), and Makefile templates.
- In 2000, Frank Warmerdam wrote Autoconf and Automake configurations for
  PROJ 4.4.0.
- This was followed by a NMake configuration to build PROJ 4.4.2 for Windows.
- In 2014, a CMake build setup was introduced by Howard Butler for
  PROJ 4.9.0RC1. The CMake configuration was improved for the 4.9.1 release,
  but not considered at feature parity with the Autotools builds at the time.
- The NMake build setup was removed for PROJ 6.0.0, as its functionality had
  been replaced by CMake.

Motivation
-------------------------------------------------------------------------------

The primary motivation in removing Autotools is to reduce the burden of
maintaining multiple build configurations, which requires developers to be
familiar with different tools and configuration files. There are several other
benefits in maintaining a single build system:

- Remove extra configuration and m4 macro files from source repository,
- Simplify scripts used for running tests for CI services (GitHub Actions,
  TravisCI),
- Reduce compilation time (and carbon footprint) used for testing on CI
  services,
- Ease development effort, particularly with new contributors.

Why drop Autotools?
-------------------------------------------------------------------------------

The GNU Build System or Autotools consist of a suite of tools including
Autoconf and Automake, which can be used to build software on Unix-like
systems. These tools are not cross-platform, and do not naively integrate
with development environments on Microsoft Windows. Furthermore, the existing
PROJ Autotools builds do not install the CMake configuration files required to
find PROJ from other projects that use CMake
(`#2546 <https://github.com/OSGeo/PROJ/issues/2546>`_).

Why use CMake?
-------------------------------------------------------------------------------

CMake is an open source cross-platform tool for build automation, testing and
packaging of software. It does not directly compile the software, but manages
the build process using generators, including Unix Makefiles and Ninja among
other command-based and IDE tools. The CMake software has been under active
development since its origins in 2000. The CMake language is carefully
developed with backwards-compatible policies that aim to provide consistent
behavior across different versions. CMake is currently the preferred build
tool for PROJ for the following reasons:

- It has existed in the PROJ code base since 2014, and is familiar to active
  PROJ contributors,
- It can install configuration files that can be used by other software that
  use CMake to find PROJ for linking via ``find_package()``,
- CMake configurations are used in 3rd-party binary packages of PROJ,
  including conda-forge and vcpkg,
- It can be used to build PROJ on all major operating systems and compiler
  combinations (where compatible),
- It has integration with modern IDEs and tools, including
  Microsoft Visual Studio and Qt Creator.

Why not CMake?
-------------------------------------------------------------------------------

Other modern cross-platform build systems exist, including Meson and Bazel,
which have many advantages over CMake. However, they are currently not widely
used by active PROJ contributors. This RFC should not restrict future build
system configurations being introduced to PROJ, if they are proven to have
advantages to CMake over time.

Potential impacts
-------------------------------------------------------------------------------

Binary packagers that currently rely on Autotools would obviously need to
transition building and testing PROJ with CMake. Issues related to
multiarch builds of PROJ may become apparent, which can be patched and/or
reported to PROJ developers. One feature of Autotools is that both static and
dynamic (shared) libraries are built, which packagers may distribute. This
feature is currently not set-up for PROJ, as it would need to be configured
and built twice.

End-users that use binary packages of PROJ should not be impacted. PROJ should
be discoverable via both pkg-config and CMake's ``find_package()``.
Other projects that use Autotools will continue to work as expected,
linking statically or dynamically to PROJ built by CMake.

Transition plan
-------------------------------------------------------------------------------

If this proposal is approved, the following tasks should be completed:

- Rewrite CI tests to only use CMake for packaging, building, testing,
  installation and post-install tests,
- Remove files only used by Autotools, also update ``.gitignore``,
- Update documentation and ``HOWTORELEASE`` notes.

Related issues will be tracked on GitHub with a tag
`RFC7: Autotools→CMake <https://github.com/OSGeo/PROJ/labels/RFC7%3A%20Autotools%E2%86%92CMake>`_.

Adoption status
-------------------------------------------------------------------------------

The RFC was adopted on 2021-10-26 with +1's from the following PSC members

* Kristian Evers
* Even Rouault
* Howard Butler
* Thomas Knudsen
* Kurt Schwehr
* Charles Karney
* Thomas Knudsen
