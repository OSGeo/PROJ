/* Projection System: default list of projections
** Use local definition of PJ_LIST_H for subset.
*/

#include "proj.h"

#define USE_PJ_LIST_H 1
#include "projects.h"


/* Generate prototypes for projection functions */
#define PROJ_HEAD(id, name) struct PJconsts *pj_##id(struct PJconsts*);
#include "pj_list.h"
#undef PROJ_HEAD

/* Generate extern declarations for description strings */
#define PROJ_HEAD(id, name) extern char * const pj_s_##id;
#include "pj_list.h"
#undef PROJ_HEAD

/* Generate the null-terminated list of projection functions with associated mnemonics and descriptions */
#define PROJ_HEAD(id, name) {#id, pj_##id, &pj_s_##id},
const struct PJ_LIST pj_list[] = {
#include "pj_list.h"
		{0,     0,  0},
	};
#undef PROJ_HEAD


struct PJ_LIST *pj_get_list_ref()
{
    return (struct PJ_LIST *)pj_list;
}

const PJ_OPERATIONS *proj_list_operations(void) {
    return pj_list;
}
