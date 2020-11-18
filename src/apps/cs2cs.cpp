/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Mainline program sort of like ``proj'' for converting between
 *           two coordinate systems.
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
 *****************************************************************************/

#define FROM_PROJ_CPP

#include <ctype.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cassert>
#include <string>

#include <proj/internal/internal.hpp>

// PROJ include order is sensitive
// clang-format off
#include "proj.h"
#include "proj_experimental.h"
#include "proj_internal.h"
#include "emess.h"
#include "utils.h"
// clang-format on

#define MAX_LINE 1000

static PJ *transformation = nullptr;

static bool srcIsGeog = false;
static double srcToRadians = 0.0;

static bool destIsGeog = false;
static double destToRadians = 0.0;
static bool destIsLatLong = false;

static int reversein = 0, /* != 0 reverse input arguments */
    reverseout = 0,       /* != 0 reverse output arguments */
    echoin = 0,           /* echo input data to output line */
    tag = '#';            /* beginning of line tag character */

static const char *oform =
    nullptr;                  /* output format for x-y or decimal degrees */
static char oform_buffer[16]; /* buffer for oform when using -d */
static const char *oterr = "*\t*"; /* output line for unprojectable input */
static const char *usage =
    "%s\nusage: %s [-dDeEfIlrstvwW [args]] [+opt[=arg] ...]\n"
    "                   [+to +opt[=arg] ...] [file ...]\n";

static double (*informat)(const char *,
                          char **); /* input data deformatter function */

/************************************************************************/
/*                              process()                               */
/*                                                                      */
/*      File processing function.                                       */
/************************************************************************/
static void process(FILE *fid)

{
    char line[MAX_LINE + 3], *s, pline[40];
    PJ_UV data;

    for (;;) {
        double z;

        ++emess_dat.File_line;
        if (!(s = fgets(line, MAX_LINE, fid)))
            break;
        if (!strchr(s, '\n')) { /* overlong line */
            int c;
            (void)strcat(s, "\n");
            /* gobble up to newline */
            while ((c = fgetc(fid)) != EOF && c != '\n')
                ;
        }
        if (*s == tag) {
            fputs(line, stdout);
            continue;
        }

        if (reversein) {
            data.v = (*informat)(s, &s);
            data.u = (*informat)(s, &s);
        } else {
            data.u = (*informat)(s, &s);
            data.v = (*informat)(s, &s);
        }

        z = strtod(s, &s);

        /* To avoid breaking existing tests, we read what is a possible t    */
        /* component of the input and rewind the s-pointer so that the final */
        /* output has consistent behavior, with or without t values.        */
        /* This is a bit of a hack, in most cases 4D coordinates will be     */
        /* written to STDOUT (except when using -E) but the output format    */
        /* specified with -f is not respected for the t component, rather it */
        /* is forward verbatim from the input.                               */
        char *before_time = s;
        double t = strtod(s, &s);
        if( s == before_time )
            t = HUGE_VAL;
        s = before_time;

        if (data.v == HUGE_VAL)
            data.u = HUGE_VAL;

        if (!*s && (s > line))
            --s; /* assumed we gobbled \n */

        if (echoin) {
            char temp;
            temp = *s;
            *s = '\0';
            (void)fputs(line, stdout);
            *s = temp;
            putchar('\t');
        }

        if (data.u != HUGE_VAL) {

            if (srcIsGeog) {
                /* dmstor gives values to radians. Convert now to the SRS unit
                 */
                data.u /= srcToRadians;
                data.v /= srcToRadians;
            }

            PJ_COORD coord;
            coord.xyzt.x = data.u;
            coord.xyzt.y = data.v;
            coord.xyzt.z = z;
            coord.xyzt.t = t;
            coord = proj_trans(transformation, PJ_FWD, coord);
            data.u = coord.xyz.x;
            data.v = coord.xyz.y;
            z = coord.xyz.z;
        }

        if (data.u == HUGE_VAL) /* error output */
            fputs(oterr, stdout);

        else if (destIsGeog && !oform) { /*ascii DMS output */

            // rtodms() expect radians: convert from the output SRS unit
            data.u *= destToRadians;
            data.v *= destToRadians;

            if (destIsLatLong) {
                if (reverseout) {
                    fputs(rtodms(pline, data.v, 'E', 'W'), stdout);
                    putchar('\t');
                    fputs(rtodms(pline, data.u, 'N', 'S'), stdout);
                } else {
                    fputs(rtodms(pline, data.u, 'N', 'S'), stdout);
                    putchar('\t');
                    fputs(rtodms(pline, data.v, 'E', 'W'), stdout);
                }
            } else if (reverseout) {
                fputs(rtodms(pline, data.v, 'N', 'S'), stdout);
                putchar('\t');
                fputs(rtodms(pline, data.u, 'E', 'W'), stdout);
            } else {
                fputs(rtodms(pline, data.u, 'E', 'W'), stdout);
                putchar('\t');
                fputs(rtodms(pline, data.v, 'N', 'S'), stdout);
            }

        } else { /* x-y or decimal degree ascii output */
            if (destIsGeog) {
                data.v *= destToRadians * RAD_TO_DEG;
                data.u *= destToRadians * RAD_TO_DEG;
            }
            if (reverseout) {
                printf(oform, data.v);
                putchar('\t');
                printf(oform, data.u);
            } else {
                printf(oform, data.u);
                putchar('\t');
                printf(oform, data.v);
            }
        }

        putchar(' ');
        if (oform != nullptr)
            printf(oform, z);
        else
            printf("%.3f", z);
        if (s)
            printf("%s", s);
        else
            printf("\n");
        fflush(stdout);
    }
}

/************************************************************************/
/*                          instantiate_crs()                           */
/************************************************************************/

static PJ *instantiate_crs(const std::string &definition,
                               bool &isGeog, double &toRadians,
                               bool &isLatFirst) {
    PJ *crs = proj_create(nullptr,
                          pj_add_type_crs_if_needed(definition).c_str());
    if (!crs) {
        return nullptr;
    }

    isGeog = false;
    toRadians = 0.0;
    isLatFirst = false;

    auto type = proj_get_type(crs);
    if (type == PJ_TYPE_BOUND_CRS) {
        auto base = proj_get_source_crs(nullptr, crs);
        proj_destroy(crs);
        crs = base;
        type = proj_get_type(crs);
    }
    if (type == PJ_TYPE_GEOGRAPHIC_2D_CRS ||
        type == PJ_TYPE_GEOGRAPHIC_3D_CRS) {
        auto cs = proj_crs_get_coordinate_system(nullptr, crs);
        assert(cs);

        isGeog = true;
        const char *axisName = "";
        proj_cs_get_axis_info(nullptr, cs, 0,
                                  &axisName, // name,
                                  nullptr,   // abbrev
                                  nullptr,   // direction
                                  &toRadians,
                                  nullptr, // unit name
                                  nullptr, // unit authority
                                  nullptr  // unit code
                                  );
        isLatFirst =
            NS_PROJ::internal::ci_find(std::string(axisName), "latitude") !=
            std::string::npos;

        proj_destroy(cs);
    }

    return crs;
}

/************************************************************************/
/*               get_geog_crs_proj_string_from_proj_crs()               */
/************************************************************************/

static std::string get_geog_crs_proj_string_from_proj_crs(PJ *src,
                                                          double &toRadians,
                                                          bool &isLatFirst) {
    auto srcType = proj_get_type(src);
    if (srcType != PJ_TYPE_PROJECTED_CRS) {
        return std::string();
    }

    auto base = proj_get_source_crs(nullptr, src);
    assert(base);
    auto baseType = proj_get_type(base);
    if (baseType != PJ_TYPE_GEOGRAPHIC_2D_CRS &&
        baseType != PJ_TYPE_GEOGRAPHIC_3D_CRS) {
        proj_destroy(base);
        return std::string();
    }

    auto cs = proj_crs_get_coordinate_system(nullptr, base);
    assert(cs);

    const char *axisName = "";
    proj_cs_get_axis_info(nullptr, cs, 0,
                              &axisName, // name,
                              nullptr,   // abbrev
                              nullptr,   // direction
                              &toRadians,
                              nullptr, // unit name
                              nullptr, // unit authority
                              nullptr  // unit code
                              );
    isLatFirst = NS_PROJ::internal::ci_find(std::string(axisName),
                                            "latitude") != std::string::npos;

    proj_destroy(cs);

    auto retCStr = proj_as_proj_string(nullptr, base, PJ_PROJ_5, nullptr);
    std::string ret(retCStr ? retCStr : "");
    proj_destroy(base);
    return ret;
}

/************************************************************************/
/*                                main()                                */
/************************************************************************/

int main(int argc, char **argv) {
    char *arg;
    char **eargv = argv;
    std::string fromStr;
    std::string toStr;
    FILE *fid;
    int eargc = 0, mon = 0;
    int have_to_flag = 0, inverse = 0;
    int use_env_locale = 0;

    /* This is just to check that pj_init() is locale-safe */
    /* Used by nad/testvarious */
    if (getenv("PROJ_USE_ENV_LOCALE") != nullptr)
        use_env_locale = 1;

    /* Enable compatibility mode for init=epsg:XXXX by default */
    if (getenv("PROJ_USE_PROJ4_INIT_RULES") == nullptr) {
        proj_context_use_proj4_init_rules(nullptr, true);
    }

    if ((emess_dat.Prog_name = strrchr(*argv, DIR_CHAR)) != nullptr)
        ++emess_dat.Prog_name;
    else
        emess_dat.Prog_name = *argv;
    inverse = !strncmp(emess_dat.Prog_name, "inv", 3);
    if (argc <= 1) {
        (void)fprintf(stderr, usage, pj_get_release(), emess_dat.Prog_name);
        exit(0);
    }

    // First pass to check if we have "cs2cs [-bla]* <SRC> <DEST> [<filename>]" syntax
    bool isProj4StyleSyntax = false;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '+') {
            isProj4StyleSyntax = true;
            break;
        }
    }

    /* process run line arguments */
    while (--argc > 0) { /* collect run line arguments */
        if (**++argv == '-') {
            for (arg = *argv;;) {
                switch (*++arg) {
                case '\0': /* position of "stdin" */
                    if (arg[-1] == '-')
                        eargv[eargc++] = const_cast<char *>("-");
                    break;
                case 'v': /* monitor dump of initialization */
                    mon = 1;
                    continue;
                case 'I': /* alt. method to spec inverse */
                    inverse = 1;
                    continue;
                case 'E': /* echo ascii input to ascii output */
                    echoin = 1;
                    continue;
                case 't': /* set col. one char */
                    if (arg[1])
                        tag = *++arg;
                    else
                        emess(1, "missing -t col. 1 tag");
                    continue;
                case 'l': /* list projections, ellipses or units */
                    if (!arg[1] || arg[1] == 'p' || arg[1] == 'P') {
                        /* list projections */
                        const struct PJ_LIST *lp;
                        int do_long = arg[1] == 'P', c;
                        const char *str;

                        for (lp = proj_list_operations(); lp->id; ++lp) {
                            (void)printf("%s : ", lp->id);
                            if (do_long) /* possibly multiline description */
                                (void)puts(*lp->descr);
                            else { /* first line, only */
                                str = *lp->descr;
                                while ((c = *str++) && c != '\n')
                                    putchar(c);
                                putchar('\n');
                            }
                        }
                    } else if (arg[1] == '=') { /* list projection 'descr' */
                        const struct PJ_LIST *lp;

                        arg += 2;
                        for (lp = proj_list_operations(); lp->id; ++lp)
                            if (!strcmp(lp->id, arg)) {
                                (void)printf("%9s : %s\n", lp->id, *lp->descr);
                                break;
                            }
                    } else if (arg[1] == 'e') { /* list ellipses */
                        const struct PJ_ELLPS *le;

                        for (le = proj_list_ellps(); le->id; ++le)
                            (void)printf("%9s %-16s %-16s %s\n", le->id,
                                         le->major, le->ell, le->name);
                    } else if (arg[1] == 'u') { /* list units */
                        auto units = proj_get_units_from_database(nullptr, nullptr, "linear", false, nullptr);
                        for( int i = 0; units && units[i]; i++ )
                        {
                            if( units[i]->proj_short_name )
                            {
                                (void)printf("%12s %-20.15g %s\n",
                                                units[i]->proj_short_name,
                                                units[i]->conv_factor,
                                                units[i]->name);
                            }
                        }
                        proj_unit_list_destroy(units);
                    } else if (arg[1] == 'm') { /* list prime meridians */
                        const struct PJ_PRIME_MERIDIANS *lpm;

                        for (lpm = proj_list_prime_meridians(); lpm->id; ++lpm)
                            (void)printf("%12s %-30s\n", lpm->id, lpm->defn);
                    } else
                        emess(1, "invalid list option: l%c", arg[1]);
                    exit(0);
                    /* cppcheck-suppress duplicateBreak */
                    continue; /* artificial */
                case 'e':     /* error line alternative */
                    if (--argc <= 0)
                    noargument:
                    emess(1, "missing argument for -%c", *arg);
                    oterr = *++argv;
                    continue;
                case 'W': /* specify seconds precision */
                case 'w': /* -W for constant field width */
                {
                    char c = arg[1];
                    if (c != 0 && isdigit(c)) {
                        set_rtodms(c - '0', *arg == 'W');
                        ++arg;
                    } else
                        emess(1, "-W argument missing or non-digit");
                    continue;
                }
                case 'f': /* alternate output format degrees or xy */
                    if (--argc <= 0)
                        goto noargument;
                    oform = *++argv;
                    continue;
                case 'r': /* reverse input */
                    reversein = 1;
                    continue;
                case 's': /* reverse output */
                    reverseout = 1;
                    continue;
                case 'D': /* set debug level */
                {
                    if (--argc <= 0)
                        goto noargument;
                    int log_level = atoi(*++argv);
                    if (log_level <= 0) {
                        proj_log_level(pj_get_default_ctx(), PJ_LOG_NONE);
                    } else if (log_level == 1) {
                        proj_log_level(pj_get_default_ctx(), PJ_LOG_ERROR);
                    } else if (log_level == 2) {
                        proj_log_level(pj_get_default_ctx(), PJ_LOG_DEBUG);
                    } else if (log_level == 3) {
                        proj_log_level(pj_get_default_ctx(), PJ_LOG_TRACE);
                    } else {
                        proj_log_level(pj_get_default_ctx(), PJ_LOG_TELL);
                    }
                    continue;
                }
                case 'd':
                    if (--argc <= 0)
                        goto noargument;
                    sprintf(oform_buffer, "%%.%df", atoi(*++argv));
                    oform = oform_buffer;
                    break;
                default:
                    emess(1, "invalid option: -%c", *arg);
                    break;
                }
                break;
            }
        } else if (!isProj4StyleSyntax) {
            if (fromStr.empty())
                fromStr = *argv;
            else if( toStr.empty() )
                toStr = *argv;
            else {
                 /* assumed to be input file name(s) */
                eargv[eargc++] = *argv;
            }
        } else if (strcmp(*argv, "+to") == 0) {
            have_to_flag = 1;

        } else if (**argv == '+') { /* + argument */
            if (have_to_flag) {
                if (!toStr.empty())
                    toStr += ' ';
                toStr += *argv;
            } else {
                if (!fromStr.empty())
                    fromStr += ' ';
                fromStr += *argv;
            }
        } else if (!have_to_flag) {
            fromStr = *argv;
        } else if (toStr.empty()) {
            toStr = *argv;
        } else /* assumed to be input file name(s) */
            eargv[eargc++] = *argv;
    }
    if (eargc == 0) /* if no specific files force sysin */
        eargv[eargc++] = const_cast<char *>("-");

    if( oform ) {
        if( !validate_form_string_for_numbers(oform) ) {
            emess(3, "invalid format string");
            exit(0);
        }
    }

    /*
     * If the user has requested inverse, then just reverse the
     * coordinate systems.
     */
    if (inverse) {
        std::swap(fromStr, toStr);
    }

    if (use_env_locale) {
        /* Set locale from environment */
        setlocale(LC_ALL, "");
    }

    if (fromStr.empty() && toStr.empty()) {
        emess(3, "missing source and target coordinate systems");
    }

    proj_context_use_proj4_init_rules(nullptr,
        proj_context_get_use_proj4_init_rules(nullptr, TRUE));

    PJ *src = nullptr;
    if (!fromStr.empty()) {
        bool ignored;
        src = instantiate_crs(fromStr, srcIsGeog,
                              srcToRadians, ignored);
        if (!src) {
            emess(3, "cannot instantiate source coordinate system");
        }
    }

    PJ *dst = nullptr;
    if (!toStr.empty()) {
        dst = instantiate_crs(toStr, destIsGeog,
                              destToRadians, destIsLatLong);
        if (!dst) {
            emess(3, "cannot instantiate target coordinate system");
        }
    }

    if (toStr.empty()) {
        assert(src);
        toStr = get_geog_crs_proj_string_from_proj_crs(src, destToRadians,
                                                       destIsLatLong);
        if (toStr.empty()) {
            emess(3,
                  "missing target CRS and source CRS is not a projected CRS");
        }
        destIsGeog = true;
    } else if (fromStr.empty()) {
        assert(dst);
        bool ignored;
        fromStr =
            get_geog_crs_proj_string_from_proj_crs(dst, srcToRadians, ignored);
        if (fromStr.empty()) {
            emess(3,
                  "missing source CRS and target CRS is not a projected CRS");
        }
        srcIsGeog = true;
    }
    proj_destroy(src);
    proj_destroy(dst);

    src = proj_create(nullptr, pj_add_type_crs_if_needed(fromStr).c_str());
    dst = proj_create(nullptr, pj_add_type_crs_if_needed(toStr).c_str());

    if( proj_get_type(src) == PJ_TYPE_COMPOUND_CRS ||
        proj_get_type(dst) == PJ_TYPE_COMPOUND_CRS ) {
        auto src3D = proj_crs_promote_to_3D(nullptr, nullptr, src);
        if( src3D ) {
            proj_destroy(src);
            src = src3D;
        }

        auto dst3D = proj_crs_promote_to_3D(nullptr, nullptr, dst);
        if( dst3D ) {
            proj_destroy(dst);
            dst = dst3D;
        }
    }

    transformation = proj_create_crs_to_crs_from_pj(nullptr, src, dst,
                                                    nullptr, nullptr);

    proj_destroy(src);
    proj_destroy(dst);

    if (!transformation) {
        emess(3, "cannot initialize transformation\ncause: %s",
              proj_errno_string(proj_context_errno(nullptr)));
    }

    if (use_env_locale) {
        /* Restore C locale to avoid issues in parsing/outputting numbers*/
        setlocale(LC_ALL, "C");
    }

    if (mon) {
        printf("%c ---- From Coordinate System ----\n", tag);
        printf("%s\n", fromStr.c_str());
        printf("%c ---- To Coordinate System ----\n", tag);
        printf("%s\n", toStr.c_str());
    }

    /* set input formatting control */
    if (!srcIsGeog)
        informat = strtod;
    else {
        informat = dmstor;
    }

    if (!destIsGeog && !oform)
        oform = "%.2f";

    /* process input file list */
    for (; eargc--; ++eargv) {
        if (**eargv == '-') {
            fid = stdin;
            emess_dat.File_name = const_cast<char *>("<stdin>");

        } else {
            if ((fid = fopen(*eargv, "rt")) == nullptr) {
                emess(-2, *eargv, "input file");
                continue;
            }
            emess_dat.File_name = *eargv;
        }
        emess_dat.File_line = 0;
        process(fid);
        fclose(fid);
        emess_dat.File_name = nullptr;
    }

    proj_destroy(transformation);

    proj_cleanup();

    exit(0); /* normal completion */
}
