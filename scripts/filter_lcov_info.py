#!/usr/bin/env python

# Remove coverage from system include files in lcov output

import sys
import fnmatch

extension_filter = None
if len(sys.argv) == 2:
    extension_filter = sys.argv[1].split(',')

output_lines = True
for line in sys.stdin.readlines():
    if line.startswith('SF:/usr/include'):
        output_lines = False
    elif line.startswith('SF:'):
        if extension_filter:
            output_lines = False
            for filter in extension_filter:
                if fnmatch.fnmatch(line.strip('\n'), filter):
                    output_lines = True
                    break
        else:
            output_lines = True
    if output_lines:
        sys.stdout.write(line)
