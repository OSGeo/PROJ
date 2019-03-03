###############################################################################
#
#  Project:  PROJ
#  Purpose: Configure custom bibliography style for sphinxcontrib-bibtex
#  Author:  Mike Toews <mwtoews at gmail.com>
#
###############################################################################
#  Copyright (c) 2018, Mike Toews <mwtoews at gmail.com>
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

import re
from pybtex.style.formatting.unsrt import Style as UnsrtStyle
from pybtex.style.labels.alpha import LabelStyle
from pybtex.plugin import register_plugin


class LinkLabelStyle(LabelStyle):
    """Citation label used in text, and before each item in the
    References section"""

    re_char_nums = re.compile(r'^[\d\w]+$')

    def format_label(self, entry):
        """Returns BibTeX key for label

        Raises KeyError if BibTeX key has other characters other than letters
        and numbers.
        """
        label = entry.key
        if not self.re_char_nums.match(label):
            raise KeyError(
                'BibTeX key must contain only letters and numbers '
                '(found {0!r})'.format(label))
        return label


class CustomStyle(UnsrtStyle):
    """Citation style in the References section"""
    # TODO: Make more Harvard-like, i.e. year after name(s)

    default_label_style = 'linklabel'
    default_name_style = 'lastfirst'
    default_sorting_style = 'author_year_title'

    def __init__(self, **kwargs):
        kwargs['abbreviate_names'] = True
        UnsrtStyle.__init__(self, **kwargs)


register_plugin('pybtex.style.labels', 'linklabel', LinkLabelStyle)
register_plugin('pybtex.style.formatting', 'customstyle', CustomStyle)
