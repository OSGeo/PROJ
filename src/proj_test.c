#include "proj.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
    int nerrors = 0;
    {
        PJ_CONTEXT* ctx;
        char out_full_filename[512];
        int ret;
        FILE* f;

        out_full_filename[0] = 0;
        setenv("PROJ_LIB", "/tmp", 1);
        f = fopen("/tmp/dummy_for_proj_test", "wb");
        if( f )
        {
            fclose(f);
            ctx = proj_context_create();
            ret = proj_find_file(ctx, "dummy_for_proj_test",
                                 out_full_filename, sizeof(out_full_filename));
            proj_context_destroy(ctx);
            if (ret != 1)
            {
                fprintf(stderr, "proj_find_file didn't return 1\n");
                nerrors ++;
            }
            if( strcmp(out_full_filename, "/tmp/dummy_for_proj_test") != 0 )
            {
                fprintf(stderr,
                        "proj_find_file() should have set "
                        "out_full_filename = %s, but it set %s instead\n",
                        "/tmp/dummy_for_proj_test",
                        out_full_filename);
                nerrors++;
            }
            unlink("/tmp/dummy_for_proj_test");
        }
    }
    return nerrors;
}
