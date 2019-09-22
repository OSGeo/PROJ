/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  Utilities for command line arguments
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2019, Even Rouault <even dot rouault at spatialys dot com>
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
 ****************************************************************************/

#include "utils.h"

#include <string.h>

bool validate_form_string_for_numbers(const char* formatString) {
    /* Only accepts '%[+]?[number]?[.]?[number]?[e|E|f|F|g|G]' */
    bool valid = true;
    if( formatString[0] != '%' )
        valid = false;
    else {
        auto oformLen = strlen(formatString);
        for( int i = 1; i < static_cast<int>(oformLen) - 1; i++ ) {
            if( !(formatString[i] == '.' ||
                    formatString[i] == '+' || 
                    (formatString[i] >= '0' && formatString[i] <= '9')) ) {
                valid = false;
                break;
            }
        }
        if( valid ) {
            valid = formatString[oformLen-1] == 'e' ||
                        formatString[oformLen-1] == 'E' ||
                        formatString[oformLen-1] == 'f' ||
                        formatString[oformLen-1] == 'F' ||
                        formatString[oformLen-1] == 'g' ||
                        formatString[oformLen-1] == 'G';
        }
    }
    return valid;
}
