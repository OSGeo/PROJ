/*
 * Copyright (c) 2026 Weston James Renoud
 *
 * Implementation of Transverse Mercator Zoned Grid System
 *
 * See IOGP Guidance Note 7-2, Section 3.2.3.2
 */

#include <errno.h>
#include <math.h>

#include "proj.h"
#include "proj_internal.h"

PROJ_HEAD(tmerczoned, "Transverse Mercator Zoned Grid System");

PJ *PJ_PROJECTION(tmerczoned) {
    proj_log_error(P, _("Not Implemented"));
    return pj_default_destructor(P, PROJ_ERR_OTHER /*ENOMEM*/);
}
