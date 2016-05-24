/***********************************************************************

               Elemental datum shift operations

                 Thomas Knudsen, 2016-05-24

************************************************************************

**Elements**

************************************************************************

Thomas Knudsen, thokn@sdfe.dk, 2016-05-24

***********************************************************************/

#define PJ_LIB__
#include <projects.h>
#include <geocent.h>
#include <PJ_pipeline.h>
#include <assert.h>
#include <stddef.h>
#include <errno.h>
PROJ_HEAD(add,     "Add a constant to the given coordinate (3 parameter shift)");



/* selftest stub */
int pj_add_selftest (void) {return 0;}
