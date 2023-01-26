.. _development_practices:

================================================================================
Development practices
================================================================================

Commit messages
^^^^^^^^^^^^^^^

Commit messages should indicate a component name (eg a driver name), a short
description, and when relevant, a reference to a issue (with 'fixes #' if it
actually fixes it)

::

    COMPONENT_NAME: fix bla bla (fixes #1234)

    Details here...

Commit hooks
^^^^^^^^^^^^

PROJ provides pre-commit hooks to run code linters before a commit is made. The
hooks are cloned with the repository and can be installed using
`pre-commit <https://pre-commit.com>`_:

.. code-block:: bash

    python -m pip install pre-commit
    pre-commit install


Once installed, the hooks can be run manually via ``pre-commit run --all-files``.

Blame ignore file
^^^^^^^^^^^^^^^^^

Due to whole-tree code reformatting done during PROJ 9.2 development,
``git blame`` information might be misleading. To avoid that, you need
to modify your git configuration as following to ignore the revision of
the whole-tree reformatting:

.. code-block:: bash

    git config blame.ignoreRevsFile .git-blame-ignore-revs
