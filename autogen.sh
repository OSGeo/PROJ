#!/bin/sh
#
# Run this to generate all the initial makefiles, etc.
#

PROJECT="PROJ.4"

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo "*** ERROR: GNU Autoconf is required to build $PROJECT."
    exit 1
}

(libtool --version) < /dev/null > /dev/null 2>&1 || {
	echo "*** ERROR: GNU Libtool is required to build $PROJECT."
    exit 1
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
	echo "*** ERROR: GNU Automake is required to build $PROJECT."
    exit 1
}

(aclocal --version) < /dev/null > /dev/null 2>&1 || {
	echo "*** ERROR: aclocal program from GNU Automake package is required to build $PROJECT."
    exit 1
}

echo "Running aclocal..."
aclocal
echo "Running libtoolize..."
libtoolize --force --copy
echo "Running automake..."
automake --add-missing --copy
echo "Running autoconf..."
autoconf

echo
echo "Type './configure' to generate Makefile files for $PROJECT."
echo "or './configure --help' to list possible options."
