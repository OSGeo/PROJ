.. _for_proj_contributors:

================================================================================
Development rules and tools for PROJ code contributors
================================================================================

This is a guide for PROJ, casual or regular, code contributors.

Coding rules
###############################################################################

To be formalized. Current rule is do like existing surrounding code...

Tools
###############################################################################

cppcheck static analyzer
--------------------------------------------------------------------------------

You can run locally ``scripts/cppcheck.sh`` that is a wrapper script around the
cppcheck utility. It is known to work with cppcheck 1.61 of Ubuntu Trusty 14.0,
since this is what is currently used on Travis-CI
(``travis/linux_gcc/before_install.sh``).
At the time of writing, this also works with cppcheck 1.72 of Ubuntu Xenial
16.04, and latest cppcheck
master.

cppcheck can have false positives. In general, it is preferable to rework the
code a bit to make it more 'obvious' and avoid those false positives. When not
possible, you can add a comment in the code like 

::

    /* cppcheck-suppress duplicateBreak */

in the preceding line. Replace
duplicateBreak with the actual name of the violated rule emitted by cppcheck.

CLang Static Analyzer (CSA)
--------------------------------------------------------------------------------

CSA is run by the ``travis/csa`` build configuration. You may also run it locally.

Preliminary step: install clang. For example:

::

    wget http://releases.llvm.org/6.0.0/clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04.tar.xz
    tar xJf clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04.tar.xz

Run configure under the scan-build utility of clang:

::

    ./clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04/bin/scan-build ./configure

Build under scan-build:

::

    ./clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04/bin/scan-build make [-j8]

If CSA finds errors, they will be emitted during the build. And in which case,
at the end of the build process, scan-build will emit a warning message
indicating errors have been found and how to display the error report. This
is with someling like

::

    ./clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04/bin/scan-view /tmp/scan-build-2018-03-15-121416-17476-1


This will open a web browser with the interactive report.

CSA may also have false positives. In general, this happens when the code is
non-trivial / makes assumptions that hard to check at first sight. You will
need to add extra checks or rework it a bit to make it more "obvious" for CSA.
This will also help humans reading your code !

Typo detection and fixes
--------------------------------------------------------------------------------

Run ``scripts/fix_typos.sh``
