# =====================================================================================
#  
#           FILE:  Makefile
#  
#    DESCRIPTION:  
# 
#          NOTES:  ---
#         AUTHOR:  Antonello Andrea
#          EMAIL:  andrea.antonello@hydrologis.com
#        COMPANY:  HydroloGIS / Engineering, University of Trento / CUDAM
#      COPYRIGHT:  Copyright (C) HydroloGIS, GPL
#        VERSION:  1.0
#        CREATED:  10/18/2004
#       REVISION:  ---
#  =====================================================================================*/
# 
# 
#  
#   This library is free software; you can redistribute it and/or 
#   modify it under the terms of the GNU Library General Public 
#   License as published by the Free Software Foundation; either 
#   version 2 of the License, or (at your option) any later version. 
# 
#   This library is distributed in the hope that it will be useful, 
#   but WITHOUT ANY WARRANTY; without even the implied warranty of 
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
#   Library General Public License for more details. 
# 
#   You should have received a copy of the GNU Library General Public 
#   License along with this library; if not, write to the Free 
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 
#   USA 
# 
# 1. Redistributions of source code must retain the above copyright   
#    notice, this list of conditions and the following disclaimer.   
# 2. Redistributions in binary form must reproduce the above copyright   
#    notice, this list of conditions and the following disclaimer in the   
#    documentation and/or other materials provided with the distribution.   
 


## JAVA related vars
# jdk_home=$(JDK_HOME)
jdk_home=/usr/java/j2sdk1.4.2_05/

#
# Windows (no idea on how to do this well)
#
ifeq ($(findstring Windows_, $(OS)), Windows_)
    OS_DEFINED = GNUmakefile
    ARCH = i386
    OSNAME = WIN32
    COMPILER = VC
else
#
# Linux
#
ifeq ($(findstring inux, $(OSTYPE)), inux)
	LIBEXT:=so
	# instructs gcc to produce shared library instead of executable module
	SO_FLAGS = -shared   # Linux 2.x
	JNI_H = -I$(jdk_home)/include/linux/ -I$(jdk_home)/include/  
    COMPILER= gcc
else

#
# Solaris (no idea on how to do this well)
#
ifeq ($(OS), Solaris)
    COMPILER = CC
else
#
# Mac OS X
#
ifeq ($(findstring darwin, $(OSTYPE)), darwin)
    LIBEXT:=a
	# instructs gcc to produce shared library instead of executable module
	SO_FLAGS = -bundle - framework System   # MacOsX
	# location of OS dependent JNI headers for C programs
	JNI_H = /System/Library/Frameworks/JavaVM.framework/1.3.1/Headers/  # MacOsX
    COMPILER = cc
else
    COMPILER = baubau
endif
endif
endif
endif


#### ######################################################################
####
#### Set values of the following variables to match your system ###########



## PROJ-related variables ####
# directory in the PROJ source tree that contains header files
# PROJINCLUDES = -I..

# directory in the PROJ source tree that contains the proj libs to be included
# PROJLIBDIR = -L../.libs

# libraries to include
LIBS = -lproj

# source directory
SRC_DIR = native

## other variables ####
DEST  = .
DEBUG = -g
DEST_LIB= libs

## C-related settings ####
CC = $(COMPILER) $(DEBUG)

#### This is the end of configuration section ##############################
####
############################################################################

# the C sources 
SRC = ${SRC_DIR}/jniproj.c

# Standard Targets 

# makes all libraries and executables with debug option if not unset
# does not make any of documentation
all: libjproj.$(LIBEXT)

libjproj.$(LIBEXT): ${SRC} 
	$(CC) $(SO_FLAGS) -o ${DEST_LIB}/$@  $(SRC) ${PROJINCLUDES} ${JNI_H} $(PROJLIBDIR) $(LIBS) 
