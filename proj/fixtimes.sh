#!/bin/sh

TARGET_FILES="configure aclocal.m4 libtool src/proj_config.h.in"
#rm -f $TARGET_FILES

cvs update $TARGET_FILES

touch $TARGET_FILES 

if test -x config.status ; then
 touch config.status
fi

touch Makefile.in */Makefile.in
