#!/bin/bash

set -e

export CCACHE_CPP2=yes

# prepare wine environment
wine64 cmd /c dir
ln -s /usr/lib/gcc/x86_64-w64-mingw32/4.8/libstdc++-6.dll  $HOME/.wine/drive_c/windows
ln -s /usr/lib/gcc/x86_64-w64-mingw32/4.8/libgcc_s_sjlj-1.dll  $HOME/.wine/drive_c/windows
ln -s /usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll  $HOME/.wine/drive_c/windows

# build sqlite3
wget https://sqlite.org/2018/sqlite-autoconf-3250100.tar.gz
tar xzf sqlite-autoconf-3250100.tar.gz
(cd sqlite-autoconf-3250100 &&  CC="ccache x86_64-w64-mingw32-gcc" CXX="ccache x86_64-w64-mingw32-g++" LD=x86_64-w64-mingw32-ld ./configure --host=x86_64-w64-mingw32 --prefix=/usr/x86_64-w64-mingw32 && make -j2 && sudo make install)
ln -s /usr/x86_64-w64-mingw32/bin/libsqlite3-0.dll  $HOME/.wine/drive_c/windows

# prepare build files
./autogen.sh
# autoconf build
mkdir build_autoconf
cd build_autoconf
CC="ccache x86_64-w64-mingw32-gcc" CXX="ccache x86_64-w64-mingw32-g++" LD=x86_64-w64-mingw32-ld ../configure --host=x86_64-w64-mingw32 --prefix=/tmp/proj_autoconf_install
make -j2
make install
make dist-all
find /tmp/proj_autoconf_install
(cd test; make -j2)
test/unit/test_cpp_api.exe
cd ..
# Now with grids
wget http://download.osgeo.org/proj/proj-datumgrid-1.5.zip
cd data
unzip -o ../proj-datumgrid-1.5.zip
cd ..
# autoconf build with grids
mkdir build_autoconf_grids
cd build_autoconf_grids
CC="ccache x86_64-w64-mingw32-gcc" CXX="ccache x86_64-w64-mingw32-g++" LD=x86_64-w64-mingw32-ld ../configure --host=x86_64-w64-mingw32 --prefix=/tmp/proj_autoconf_install_nad
make -j2
make install
find /tmp/proj_autoconf_install_nad
#make check
cd src
make multistresstest.exe
make test228.exe
cd ..
PROJ_LIB=../data src/multistresstest.exe -num_iterations 100000
cd ..

