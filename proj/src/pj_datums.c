/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Built in datum list.
 * Author:   Frank Warmerdam, warmerda@home.com
 *
 ******************************************************************************
 * Copyright (c) 2000, Frank Warmerdam
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 *
 * $Log$
 * Revision 1.1  2000/07/06 23:32:27  warmerda
 * New
 *
 */

#define PJ_DATUMS__

#include <projects.h>

/* 
 * The ellipse code must match one from pj_ellps.c.  The datum id should
 * be kept to 12 characters or less if possible.  Use the official OGC 
 * datum name for the comments if available. 
 */

struct PJ_DATUMS pj_datums[] = {
/* id       definition                               ellipse  comments */
/* --       ----------                               -------  -------- */
"WGS84",    "towgs84=0,0,0", 		             "WGS84", "",
"GGRS87",   "towgs84=-199.87,74.79,246.62",          "GRS80", 
				"Greek_Geodetic_Reference_System_1987",
"NAD83",    "towgs84=0,0,0",                         "GRS80", 
				"North_American_Datum_1983",
"NAD27",    "nadgrids=conus",                        "clrk66", 
				"North_American_Datum_1927",
NULL,       NULL,                                    NULL,    NULL 
};
