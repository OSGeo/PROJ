##
# OSGeo/PROJ

FROM ubuntu:18.04 as builder

MAINTAINER Howard Butler <howard@hobu.co>

ARG PROJ_VERSION=master
ARG DESTDIR="/build"

# Setup build env
RUN apt-get update -y \
    && apt-get install -y --fix-missing --no-install-recommends \
            software-properties-common build-essential ca-certificates \
            git make cmake wget unzip libtool automake \
            zlib1g-dev libsqlite3-dev pkg-config sqlite3

RUN git clone https://github.com/OSGeo/PROJ.git \
    && cd PROJ \
    && ./autogen.sh \
    && ./configure --prefix=/usr \
    && make \
    && make install




FROM ubuntu:18.04 as runner

RUN date

RUN apt-get update; \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
        libsqlite3-0 \
        curl unzip

# Put this first as this is rarely changing
RUN \
    mkdir -p /usr/share/proj; \
    curl -LOs https://download.osgeo.org/proj/proj-datumgrid-1.8.zip &&  unzip -j -u -o proj-datumgrid-1.8.zip  -d /usr/share/proj; \
    curl -LOs https://download.osgeo.org/proj/proj-datumgrid-europe-1.2.zip &&  unzip -j -u -o proj-datumgrid-europe-1.2.zip -d /usr/share/proj; \
    curl -LOs https://download.osgeo.org/proj/proj-datumgrid-oceania-1.0.zip &&  unzip -j -u -o proj-datumgrid-oceania-1.0.zip -d /usr/share/proj; \
    curl -LOs https://download.osgeo.org/proj/proj-datumgrid-world-1.0.zip &&  unzip -j -u -o proj-datumgrid-world-1.0.zip -d /usr/share/proj; \
    curl -LOs https://download.osgeo.org/proj/proj-datumgrid-north-america-1.2.zip &&  unzip -j -u -o proj-datumgrid-north-america-1.2.zip -d /usr/share/proj; \
    rm *.zip

COPY --from=builder  /build/usr/share/proj/ /usr/share/proj/
COPY --from=builder  /build/usr/include/ /usr/include/
COPY --from=builder  /build/usr/bin/ /usr/bin/
COPY --from=builder  /build/usr/lib/ /usr/lib/

