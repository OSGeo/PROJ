.. _errorhandling:

================================================================================
Error handling
================================================================================

PROJ maintains an internal error state, which is local to a
:c:type:`PJ_CONTEXT` thread context.

See :doc:`quickstart` for more information about how to create and use a thread
context object.

If you receive an abnormal return from a PROJ API function (e.g. a NULL
pointer) you may wish to discover more information about the error.

In this case you can make a call to :c:func:`proj_context_errno`, passing in
your thread context.  This will return an integer error code.

If the error code is zero, the last PROJ operation was deemed successful and no
error has been detected.

If the error code is non-zero, an error has been detected.  You can pass your
thread context together with this error code to
:c:func:`proj_context_errno_string` to retrieve a string describing the error
condition.

A basic example showing how a C program might catch and report errors follows:

.. code-block:: c
  :caption: errorhandling.c
  :linenos:

    #include <stdio.h>
    #include <proj.h>

    int main (void) {
        PJ_CONTEXT *c;
        PJ *p;
        int errno;
        const char *errstr;

        c = proj_context_create();
        p = proj_create_crs_to_crs(c, "EPSG:4326", "EPSG:3857", NULL);

        if (p == 0) {
            /* Something is wrong, let's try to get details ... */
            errno = proj_context_errno(c);
            if (errno == 0) {
                /* This should be impossible. */
                fprintf(stderr, "Failed to create transformation, reason unknown.\n");
            } else {
                errstr = proj_context_errno_string(c, errno);
                fprintf(stderr, "Failed to create transformation: %s.\n", errstr);
            }
            proj_context_destroy(c);
            return 1;
        }
        
        /* transformation object is valid, do work ... */

        proj_destroy(p);
        proj_context_destroy(c);

        return 0;
    }
