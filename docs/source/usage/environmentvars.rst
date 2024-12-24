.. _environmentvars:

================================================================================
Environment variables
================================================================================

PROJ can be controlled by setting environment variables. Most users will
have a use for the :envvar:`PROJ_DATA`.

On UNIX systems environment variables can be set for a shell-session with::

    $ export VAR="some variable"

or it can be set for just one command line call::

    $ VAR="some variable" ./cmd

Environment variables on UNIX are usually removed with the ``unset`` command::

    $ unset VAR

On windows systems environment variables can be set in the command line with::

    > set VAR="some variable"

``VAR`` will be available for the entire session, unless it is unset. This is
done by setting the variable with no content::

    > set VAR=

.. envvar:: PROJ_DATA

    Prior to PROJ 9.1, this environment variable was called :envvar:`PROJ_LIB`.
    This older name is still supported in PROJ 9.1 as a fallback, but support for it
    may be removed in future release.

    The location of PROJ :doc:`resource files<../resource_files>`.

    Starting with PROJ 6, multiple directories can be specified. On Unix, they
    should be separated by the colon (:) character. on Windows, by the semi-colon (;)
    character.

    PROJ is hardcoded to look for resource files
    in other locations as well, amongst those are the
    installation directory (usually ``share/proj`` under the PROJ
    installation root) and the current folder.

    You can also set the location of the resource files using
    :func:`proj_context_set_search_paths` in the :file:`proj.h` API header.

.. versionchanged:: 6.1.0

    Starting with PROJ version 6.1.0, the paths set by
    :func:`proj_context_set_search_paths` will have priority over the
    :envvar:`PROJ_DATA` to allow for multiple versions of PROJ
    resource files on your system without conflicting.


.. envvar:: PROJ_AUX_DB

    .. versionadded:: 8.1.0

    To set the path to one or several auxiliary SQLite3 databases of structure
    identical to the main :file:`proj.db` database and that can contain additional
    object (CRS, transformation, ...) definitions. If several paths are
    provided, they must be separated by the colon (:) character on Unix, and
    on Windows, by the semi-colon (;) character.


.. envvar:: PROJ_DEBUG

    Set the debug level of PROJ.

    The following levels are available:
    - ``0``: no message.
    - ``1``: error messages only (default).
    - ``2``: same as 1, with debug messages.
    - ``3``: same as 2, with verbose messages.
    - ``4``: same as 3, with very verbose messages.

    Starting with PROJ 9.3, ``ON`` can be used as an alias for ``2``,
    and ``OFF`` as an alias for ``1``.

.. envvar:: PROJ_NETWORK

    .. versionadded:: 7.0.0

    If set to ON, enable the capability to use remote grids stored on CDN
    (Content Delivery Network) storage, when grids are not available locally.
    Alternatively, the :c:func:`proj_context_set_enable_network` function can
    be used.

.. envvar:: PROJ_NETWORK_ENDPOINT

    .. versionadded:: 7.0.0

    Define the endpoint of the CDN storage. Normally defined through the
    :file:`proj.ini` configuration file locale in :envvar:`PROJ_DATA`.
    Alternatively, the :c:func:`proj_context_set_url_endpoint` function can
    be used.

.. envvar:: PROJ_CURL_CA_BUNDLE

    .. versionadded:: 7.2.0

    Define a custom path to the CA Bundle file. This can be useful if `curl`
    and :envvar:`PROJ_NETWORK` are enabled. Alternatively, the 
    :c:func:`proj_curl_set_ca_bundle_path` function can be used.

.. envvar:: PROJ_NATIVE_CA

    .. versionadded:: 9.6.0
    
    When this is set to ON, the operating systems native CA store will be used for certificate verification
    If you set this option to ON and also set PROJ_CURL_CA_BUNDLE then during verification those certificates are
    searched in addition to the native CA store.
