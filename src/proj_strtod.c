/***********************************************************************

 proj_strtod: Convert string to double, accepting underscore separators

                    Thomas Knudsen, 2017-01-17/09-19

************************************************************************

Conventionally, PROJ.4 does not honor locale settings, consistently
behaving as if LC_ALL=C.

For this to work, we have, for many years, been using other solutions
than the C standard library strtod/atof functions for converting strings
to doubles.

In the early versions of proj, iirc, a gnu version of strtod was used,
mostly to work around cases where the same system library was used for
C and Fortran linking, hence making strtod accept "D" and "d" as
exponentiation indicators, following Fortran Double Precision constant
syntax. This broke the proj angular syntax accepting a "d" to mean
"degree": 12d34'56", meaning 12 degrees 34 minutes and 56 seconds.

With an explicit MIT licence, PROJ.4 could not include GPL code any
longer, and apparently at some time, the GPL code was replaced by the
current C port of a GDAL function (in pj_strtod.c), which reads the
LC_NUMERIC setting and, behind the back of the user, momentarily changes
the conventional '.' delimiter to whatever the locale requires, then
calls the system supplied strtod.

While this requires a minimum amount of coding, it only solves one
problem, and not in a very generic way.

Another problem, I would like to see solved, is the handling of underscores
as generic delimiters. This is getting popular in a number of programming
languages (Ada, C++, C#, D, Java, Julia, Perl 5, Python, Rust, etc.
cf. e.g. https://www.python.org/dev/peps/pep-0515/), and in our case of
handling numbers being in the order of magnitude of the Earth's dimensions,
and a resolution of submillimetre, i.e. having 10 or more significant digits,
splitting the "wall of digits" into smaller chunks is of immense value.

Hence this reimplementation of strtod, which hardcodes '.' as indicator of
numeric fractions, and accepts '_' anywhere in a numerical string sequence:
So a typical northing value can be written

                            6_098_907.8250 m
rather than
                             6098907.8250 m

which, in my humble opinion, is well worth the effort.

While writing this code, I took ample inspiration from Michael Ringgaard's
strtod version over at http://www.jbox.dk/sanos/source/lib/strtod.c.html,
and Yasuhiro Matsumoto's public domain version over at
https://gist.github.com/mattn/1890186. The code below is, however, not
copied from any of the two mentioned - it is a reimplementation, and
probably suffers from its own set of bugs. So for now, it is intended
not as a replacement of pj_strtod, but only as an experimental piece of
code for use in an experimental new transformation program, cct.

************************************************************************

Thomas Knudsen, thokn@sdfe.dk, 2017-01-17/2017-09-18

************************************************************************

* Copyright (c) 2017 Thomas Knudsen & SDFE
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

***********************************************************************/


#include <errno.h>
#include <ctype.h>
#include <float.h>  /* for HUGE_VAL */
#include <math.h>   /* for pow() */

double proj_strtod(const char *str, char **endptr);
double proj_atof(const char *str);
    

double proj_strtod(const char *str, char **endptr) {
    double number = 0;
    int exponent = 0;
    int sign = 0;
    char *p = (char *) str;
    int n = 0;
    int num_digits_total = 0;
    int num_digits_after_comma = 0;

    if (0==str) {
        errno = EFAULT;
        if (endptr)
            *endptr = p;
        return HUGE_VAL;
    }

    /* First skip leading whitespace */
    while (isspace(*p))
        p++;

    /* Empty string? */
    if (0==*p) {
        errno = EINVAL;
        if (endptr)
            *endptr = p;
        return HUGE_VAL;
    }

    /* Then handle optional prefixed sign */
    switch (*p) {
        case '-':
            sign = -1,  p++;  break;
        case '+':
            sign =  1,  p++;  break;
        default:
            if (isdigit(*p) || '_'==*p || '.'==*p)
                break;
            if (endptr)
                *endptr = p;
            errno = EINVAL;
            return HUGE_VAL;
    }

    /* Now expect a (potentially zero-length) string of digits */
    while (isdigit(*p) || ('_'==*p)) {
        if ('_'==*p) {
            p++;
            continue;
        }
        number = number * 10. + (*p - '0');
        p++;
        num_digits_total++;
    }
        
    /* Do we have a fractional part? */
    if ('.'==*p) {
        p++;

        while (isdigit(*p) || '_'==*p) {
            if ('_'==*p) {
                p++;
                continue;
            }
            number = number * 10. + (*p - '0');
            p++;
            num_digits_total++;
            num_digits_after_comma++;
        }

        exponent = -num_digits_after_comma;
    }

    /* non-digit */
    if (0==num_digits_total) {
        errno = EINVAL;
        if (endptr)
            *endptr = p;
        return HUGE_VAL;
    }

    if (sign==-1)
        number = -number;

    /* Do we have an exponent part? */
    if (*p == 'e' || *p == 'E') {
        p++;
        /* Does it have a sign? */
        sign = 0;
        if ('-'==*p)
            sign = -1;
        if ('+'==*p)
            sign = +1;
        if (0==sign) {
            if (!isdigit(*p) && *p!='_') {
                if (endptr)
                    *endptr = p;
                return HUGE_VAL;
            }
        }
        else
            p++;


        /* Go on and read the exponent */
        n = 0;
        while (isdigit(*p) || '_'==*p) {
            if ('_'==*p) {
                p++;
                continue;
            }
            n = n * 10 + (*p - '0');
            p++;
        }

        if (-1==sign)
            n = -n;
        exponent += n;
    }

    if ((exponent < DBL_MIN_EXP) || (exponent > DBL_MAX_EXP)) {
      errno = ERANGE;
      if (endptr)
          *endptr = p;
      return HUGE_VAL;
    }

    number *= pow (10, exponent);

    if (fabs(number) > DBL_MAX)
        errno = ERANGE;

    if (endptr)
        *endptr = p;

    return number;
}

double proj_atof(const char *str) {
  return proj_strtod(str, (void *) 0);
}

#ifdef TEST
#include <string.h>

int main (int argc, char **argv) {
    double res;
    char *endptr;
    if (argc < 2)
        return 0;
    res =  proj_strtod (argv[1], &endptr);
    printf ("res = %20.15g. Rest = [%s],  errno = %d\n", res, endptr, (int) errno);
    return 0;
}
#endif
