.. _environmentvars:

================================================================================
Environment variables
================================================================================

PROJ can be controlled by setting environment variables. Most users will
have a use for the :envvar:`PROJ_LIB`.

On UNIX systems environment variables can be set for a shell-session with::

    $ export VAR="some variable"

or it can be set for just one command line call::

    $ VAR="some variable" ./cmd

Environment variables on UNIX are usually removed with the ``unset`` command::

    $ unset VAR

On windows systems environment variables can be set in the command line with::

    > set VAR="some variable"

```VAR`` will be available for the entire session, unless it is unset. This is
done by setting the variable with no content::

    > set VAR=

.. envvar:: PROJ_LIB

    The location of PROJ :doc:`resource files<../resource_files>`.
    It is only possible to specify a single library in :envvar:`PROJ_LIB`; e.g. it
    does not behave like PATH. PROJ is hardcoded to look for resource files
    in other locations as well, amongst those are the users home directory,
    ``/usr/share/proj`` and the current folder.

.. envvar:: PROJ_DEBUG

    Set the debug level of PROJ. The default debug level is zero, which results
    in no debug output when using PROJ. A number from 1-3, whit 3 being the most
    verbose setting.
