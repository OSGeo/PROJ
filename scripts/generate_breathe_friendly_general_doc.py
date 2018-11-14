#!/usr/bin/env python
###############################################################################
# $Id$
#
#  Project:  PROJ
#  Purpose:  Rework general_doc.dox in a Breathe friendly (hacky) way
#  Author:   Even Rouault <even.rouault at spatialys.com>
#
###############################################################################
#  Copyright (c) 2018, Even Rouault <even.rouault at spatialys.com>
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

import os

# HACK HACK HACK
# Transform a .dox with section, subsection, etc.. in fake namespaces
# since Breathe cannot handle such sections and references to them

script_dir_name = os.path.dirname(os.path.realpath(__file__))
general_doc_dox = os.path.join(os.path.dirname(
    script_dir_name), 'src', 'general_doc.dox')
general_doc_dox_reworked_h = os.path.join(os.path.dirname(
    script_dir_name), 'docs/build/tmp_breathe', 'general_doc.dox.reworked.h')

out = open(general_doc_dox_reworked_h, 'wt')


class Section:

    def __init__(self):
        self.name = None
        self.title = None
        self.content = ''
        self.children = []
        pass

    def serialize(self, out):
        out.write("/** \\brief %s\n" % self.title)
        out.write("\n")
        out.write("%s\n" % self.content)
        out.write("*/\n")
        out.write("namespace %s {\n" % self.name)
        out.write("}\n")
        for child in self.children:
            child.serialize(out)


stack = []
current = None

for line in open(general_doc_dox, 'rt').readlines():
    line = line.strip()
    if line == '/*!' or line == '*/':
        continue

    if line.startswith('\page'):

        tokens = line.split(' ')
        new = Section()
        new.name = tokens[1]
        new.title = ' '.join(tokens[2:])
        current = new
        stack.append(new)
        continue

    if line.startswith('\section'):

        stack = stack[0:1]
        current = stack[-1]

        tokens = line.split(' ')
        new = Section()
        new.name = tokens[1]
        new.title = ' '.join(tokens[2:])
        current.children.append(new)
        current = new
        stack.append(new)
        continue

    if line.startswith('\subsection'):

        stack = stack[0:2]
        current = stack[-1]

        tokens = line.split(' ')
        new = Section()
        new.name = tokens[1]
        new.title = ' '.join(tokens[2:])
        current.children.append(new)
        current = new
        stack.append(new)
        continue

    if line.startswith('\subsubsection'):

        stack = stack[0:3]
        current = stack[-1]

        tokens = line.split(' ')
        new = Section()
        new.name = tokens[1]
        new.title = ' '.join(tokens[2:])
        current.children.append(new)
        current = new
        stack.append(new)
        continue

    if current:
        current.content += line + '\n'

for child in stack[0].children:
    child.serialize(out)
