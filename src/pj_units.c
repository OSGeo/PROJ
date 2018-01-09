/* definition of standard cartesian units */

#include "proj.h"

#define PJ_UNITS__
#include <projects.h>

/* Field 2 that contains the multiplier to convert named units to meters
** may be expressed by either a simple floating point constant or a
** numerator/denomenator values (e.g. 1/1000) */
C_NAMESPACE_VAR const struct PJ_UNITS
pj_units[] = {
    {"km",      "1000.",                "Kilometer",                    1000.0},
    {"m",       "1.",                   "Meter",                        1.0},
    {"dm",      "1/10",                 "Decimeter",                    0.1},
    {"cm",      "1/100",                "Centimeter",                   0.01},
    {"mm",      "1/1000",               "Millimeter",                   0.001},
    {"kmi",     "1852.0",               "International Nautical Mile",  1852.0},
    {"in",      "0.0254",               "International Inch",           0.0254},
    {"ft",      "0.3048",               "International Foot",           0.3048},
    {"yd",      "0.9144",               "International Yard",           0.9144},
    {"mi",      "1609.344",             "International Statute Mile",   1609.344},
    {"fath",    "1.8288",               "International Fathom",         1.8288},
    {"ch",      "20.1168",              "International Chain",          20.1168},
    {"link",    "0.201168",             "International Link",           0.201168},
    {"us-in",   "1./39.37",             "U.S. Surveyor's Inch",         0.0254},
    {"us-ft",   "0.304800609601219",    "U.S. Surveyor's Foot",         0.304800609601219},
    {"us-yd",   "0.914401828803658",    "U.S. Surveyor's Yard",         0.914401828803658},
    {"us-ch",   "20.11684023368047",    "U.S. Surveyor's Chain",        20.11684023368047},
    {"us-mi",   "1609.347218694437",    "U.S. Surveyor's Statute Mile", 1609.347218694437},
    {"ind-yd",  "0.91439523",           "Indian Yard",                  0.91439523},
    {"ind-ft",  "0.30479841",           "Indian Foot",                  0.30479841},
    {"ind-ch",  "20.11669506",          "Indian Chain",                 20.11669506},
    {NULL,      NULL,                   NULL,                           0.0}
};

struct PJ_UNITS *pj_get_units_ref()
{
    return (struct PJ_UNITS *)pj_units;
}

const PJ_UNITS *proj_list_units()
{
    return pj_units;
}
