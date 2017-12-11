import sys
import json

def write_coords(f, coords, testtype, test):
    '''testtype can be either ('fwd', 'inv', 'roundtrip')'''

    for coord in coords:
        if len(coord[0]) == 3:
            cstring = '{} {} {}'
        else:
            cstring = '{} {}'

        f.write('tolerance  {tol} m\n'.format(tol=test['tolerances'][tolerance_index]))
        accept = 'accept    {s}\n'.format(s=cstring)
        expect = 'expect    {s}\n'.format(s=cstring)

        if testtype == 'fwd':
            f.write(accept.format(*coord[0]))
            f.write(expect.format(*coord[1]))

        if testtype == 'inv':
            f.write(accept.format(*coord[1]))
            f.write(expect.format(*coord[0]))

        if testtype == 'roundtrip':
            f.write(accept.format(*coord[0]))
            f.write('roundtrip {n}\n'.format(n=test['times']))

        f.write('\n')



with open(sys.argv[1]) as f:
    data = json.load(f)

coords = data['coordinates']
tests = data['tests']
crss = data['projections']

if len(coords[0]) == 3:
    tolerance_index = 0
else:
    tolerance_index = 1

filename = sys.argv[1].replace('json', 'gie')
with open(filename, 'w') as f:
    f.write('-'*80 + '\n\n')
    f.write('{descr}\n\n'.format(descr=data['description']))
    f.write('-'*80 + '\n\n')
    f.write('<gie>\n\n');
    for test in tests:
        f.write('-'*80 + '\n')
        f.write('operation  +proj=pipeline\n')
        f.write('           +step {src} +inv\n'.format(src=crss[0]))
        f.write('           +step {dst}\n'.format(dst=crss[1]))
        f.write('-'*80 + '\n')
        if test['type'] == 'conversion':
            write_coords(f, coords, 'fwd', test)
            f.write('-'*80 + '\n')
            f.write('operation  +proj=pipeline\n')
            f.write('           +step {src} +inv\n'.format(src=crss[1]))
            f.write('           +step {dst}\n'.format(dst=crss[0]))
            f.write('-'*80 + '\n')
            write_coords(f, coords, 'inv', test)

        if test['type'] == 'roundtrip':
            write_coords(f, coords, 'roundtrip', test)

    f.write('</gie>\n')
