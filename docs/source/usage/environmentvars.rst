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

    Starting with PROJ 6, multiple directories can be specified. On Unix, they
    should be separated by the colon (:) character. on Windows, by the semi-colon (;)
    character.

    PROJ is hardcoded to look for resource files
    in other locations as well, amongst those are the users home directory,
    ``/usr/share/proj`` and the current folder.

.. envvar:: PROJ_DEBUG

    Set the debug level of PROJ. The default debug level is zero, which results
    in no debug output when using PROJ. A number from 1-3, whit 3 being the most
    verbose setting.
