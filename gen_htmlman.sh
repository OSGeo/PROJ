#!/usr/bin/env bash

# ----------------------------------------------------------------------
#
# Generates HTML versions of man pages.  This is intended to be run when
# releasing PROJ.4.
#
# requires man2html command
#
# ----------------------------------------------------------------------

# Note: nad2nad has been deprecated in favor to using cs2cs, do not include
# nad2nad.1

# list of man pages
MANPAGES="man1/cs2cs.1
man1/proj.1
man1/geod.1
man3/pj_init.3
man3/geodesic.3"
               
# make directories
mkdir html/man1
mkdir html/man3
                
for manpg in $MANPAGES
do
    man2html -r man/$manpg > html/$manpg.html
done
