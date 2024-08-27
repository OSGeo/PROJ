#!/usr/bin/env python
###############################################################################
# $Id$
#
#  Project:  PROJ
#  Purpose:  Generate ITRF2020 to ITRFxxxx Helmert transformations (geocentric
#            coordinates) from EPSG database.
#  Author:   Even Rouault <even.rouault at spatialys.com>
#
###############################################################################
#  Copyright (c) 2024, Even Rouault <even.rouault at spatialys.com>
#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included
#  in all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
#  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
###############################################################################

import subprocess

def change_sign(param):
    tokens = param.split("=")
    if tokens[1] == "0":
        return None
    if tokens[1][0] == '-':
        return tokens[0] + "=" + tokens[1][1:]
    else:
        return tokens[0] + "=-" + tokens[1]

def generate(source, dest):

    process = subprocess.Popen(f"bin/projinfo -s {source} -t {dest} --single-line -o PROJ -q", shell=True,
                               stdout=subprocess.PIPE, 
                               stderr=subprocess.PIPE)
    out, _ = process.communicate()

    steps = out.decode('ASCII').split('+step')
    assert len(steps) == 8
    txt = steps[4]
    txt = txt[:-1]
    assert txt.startswith(" +inv +proj=helmert")
    txt = txt.replace(" +inv ", "")
    new_tokens = []
    for token in txt.split(" "):
        if token.startswith("+x=") or token.startswith("+y=") or token.startswith("+z=") or \
           token.startswith("+dx=") or token.startswith("+dy=") or token.startswith("+dz=") or \
           token.startswith("+rx=") or token.startswith("+ry=") or token.startswith("+rz=") or \
           token.startswith("+drx=") or token.startswith("+dry=") or token.startswith("+drz=") or \
           token.startswith("+s=") or token.startswith("+ds="):
            new_token = change_sign(token)
            if new_token:
                new_tokens.append(new_token)
        elif token.startswith("+proj=") or token.startswith("+t_epoch=") or token.startswith("+convention="):
            new_tokens.append(token)
        else:
            assert False, (txt, token)
    print("<" + dest + "> " + (" ".join(new_tokens)))

for dest in ("ITRF2014", "ITRF2008", "ITRF2005", "ITRF2000", "ITRF97", "ITRF96",
             "ITRF94", "ITRF93", "ITRF92", "ITRF91", "ITRF90", "ITRF89", "ITRF88"):
    generate("ITRF2020", dest)
    print("")
