##
# OSGeo/PROJ

FROM ubuntu:24.04 AS builder

LABEL maintainer="Howard Butler <howard@hobu.co>"

ARG DESTDIR="/build"

# Setup build env
RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --fix-missing --no-install-recommends \
        software-properties-common build-essential ca-certificates \
        cmake wget unzip \
        zlib1g-dev libsqlite3-dev sqlite3 libcurl4-gnutls-dev \
        libtiff-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

COPY . /PROJ

RUN cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -S /PROJ -B _build
RUN cmake --build _build -j $(nproc)
RUN cmake --install _build
RUN rm -rfv _build

FROM ubuntu:24.04 AS runner

RUN date

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        libsqlite3-0 libtiff6 libcurl4 libcurl3-gnutls \
        wget ca-certificates \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Put this first as this is rarely changing
WORKDIR /usr/share/proj
RUN wget --no-verbose --mirror https://cdn.proj.org/ || true
RUN cd cdn.proj.org && rm -fv *.js *.css *.html favicon* && mv * .. && cd .. && rmdir cdn.proj.org
WORKDIR /

COPY --from=builder  /build/usr/share/proj/ /usr/share/proj/
COPY --from=builder  /build/usr/include/ /usr/include/
COPY --from=builder  /build/usr/bin/ /usr/bin/
COPY --from=builder  /build/usr/lib/ /usr/lib/
