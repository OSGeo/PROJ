##
# OSGeo/PROJ

FROM ubuntu:20.04 as builder

MAINTAINER Howard Butler <howard@hobu.co>

ARG DESTDIR="/build"

# Setup build env
RUN apt-get update -y \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --fix-missing --no-install-recommends \
            software-properties-common build-essential ca-certificates \
            cmake wget unzip \
            zlib1g-dev libsqlite3-dev sqlite3 libcurl4-gnutls-dev \
            libtiff5-dev

COPY . /PROJ

RUN cd /PROJ \
    && mkdir build \
    && cd build \
    && cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF \
    && make -j$(nproc) \
    && make install




FROM ubuntu:20.04 as runner

RUN date

RUN apt-get update; \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
        libsqlite3-0 libtiff5 libcurl4 libcurl3-gnutls \
        wget ca-certificates

# Put this first as this is rarely changing
RUN \
    mkdir -p /usr/share/proj; \
    wget --no-verbose --mirror https://cdn.proj.org/; \
    rm -f cdn.proj.org/*.js; \
    rm -f cdn.proj.org/*.css; \
    mv cdn.proj.org/* /usr/share/proj/; \
    rmdir cdn.proj.org

COPY --from=builder  /build/usr/share/proj/ /usr/share/proj/
COPY --from=builder  /build/usr/include/ /usr/include/
COPY --from=builder  /build/usr/bin/ /usr/bin/
COPY --from=builder  /build/usr/lib/ /usr/lib/

