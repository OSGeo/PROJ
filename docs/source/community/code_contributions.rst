.. _code_contributions:

================================================================================
Guidelines for PROJ code contributors
================================================================================

This is a guide for PROJ, casual or regular, code contributors.

Code contributions.
###############################################################################

Code contributions can be either bug fixes or new features. The process
is the same for both, so they will be discussed together in this
section.

Making Changes
~~~~~~~~~~~~~~

-  Create a topic branch from where you want to base your work.
-  You usually should base your topic branch off of the master branch.
-  To quickly create a topic branch: ``git checkout -b my-topic-branch``
-  Make commits of logical units.
-  Check for unnecessary whitespace with ``git diff --check`` before
   committing.
-  Make sure your commit messages are in the `proper
   format <http://tbaggery.com/2008/04/19/a-note-about-git-commit-messages.html>`__.
-  Make sure you have added the necessary tests for your changes.
-  Make sure that all tests pass

Submitting Changes
~~~~~~~~~~~~~~~~~~

-  Push your changes to a topic branch in your fork of the repository.
-  Submit a pull request to the PROJ repository in the OSGeo
   organization.
-  If your pull request fixes/references an issue, include that issue
   number in the pull request. For example:

::

    Wiz the bang

    Fixes #123.

-  PROJ developers will look at your patch and take an appropriate
   action.

Coding conventions
~~~~~~~~~~~~~~~~~~

Programming language
^^^^^^^^^^^^^^^^^^^^

PROJ is developed strictly in ANSI C 89.

Coding style
^^^^^^^^^^^^

We don't enforce any particular coding style, but please try to keep it
as simple as possible. If improving existing code, please try to conform
with the style of the locally surrounding code.

Whitespace
^^^^^^^^^^

Throughout the PROJ code base you will see differing whitespace use.
The general rule is to keep whitespace in whatever form it is in the
file you are currently editing. If the file has a mix of tabs and space
please convert the tabs to space in a separate commit before making any
other changes. This makes it a lot easier to see the changes in diffs
when evaluating the changed code. New files should use spaces as
whitespace.

File names
^^^^^^^^^^

Files in which projections are implemented are prefixed with an
upper-case ``PJ_`` and most other files are prefixed with lower-case
``pj_``. Some file deviate from this pattern, most of them dates back to
the very early releases of PROJ. New contributions should follow the
pj-prefix pattern. Unless there are obvious reasons not to.


Tools
###############################################################################

cppcheck static analyzer
~~~~~~~~~~~~~~~~~~~~~~~~

You can run locally ``scripts/cppcheck.sh`` that is a wrapper script around the
cppcheck utility. This tool is used as part of the quality control of the code.

cppcheck can have false positives. In general, it is preferable to rework the
code a bit to make it more 'obvious' and avoid those false positives. When not
possible, you can add a comment in the code like 

::

    /* cppcheck-suppress duplicateBreak */

in the preceding line. Replace
duplicateBreak with the actual name of the violated rule emitted by cppcheck.

CLang Static Analyzer (CSA)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

CSA is run by the ``travis/csa`` build configuration. You may also run it locally.

Preliminary step: install clang. For example:

::

    wget http://releases.llvm.org/6.0.0/clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz
    tar xJf clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz
    mv clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-16.04 clang+llvm-6

Run configure under the scan-build utility of clang:

::

    ./clang+llvm-6/bin/scan-build ./configure

Build under scan-build:

::

    ./clang+llvm-6/bin/scan-build make [-j8]

If CSA finds errors, they will be emitted during the build. And in which case,
at the end of the build process, scan-build will emit a warning message
indicating errors have been found and how to display the error report. This
is with someling like

::

    ./clang+llvm-6/bin/scan-view /tmp/scan-build-2018-03-15-121416-17476-1


This will open a web browser with the interactive report.

CSA may also have false positives. In general, this happens when the code is
non-trivial / makes assumptions that hard to check at first sight. You will
need to add extra checks or rework it a bit to make it more "obvious" for CSA.
This will also help humans reading your code !

Typo detection and fixes
~~~~~~~~~~~~~~~~~~~~~~~~

Run ``scripts/fix_typos.sh``

Include What You Use (IWYU)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Managing C includes is a pain.  IWYU makes updating headers a bit
easier.  IWYU scans the code for functions that are called and makes
sure that the headers for all those functions are present and in
sorted order.  However, you cannot blindly apply IWYU to PROJ.  It
does not understand ifdefs, other platforms, or the order requirements
of PROJ internal headers.  So the way to use it is to run it on a copy
of the source and merge in only the changes that make sense.
Additions of standard headers should always be safe to merge.  The
rest require careful evaluation.  See the IWYU documentation for
motivation and details.

`IWYU docs <https://github.com/include-what-you-use/include-what-you-use/tree/master/docs>`_
