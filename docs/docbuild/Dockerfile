FROM ubuntu:24.04

RUN apt-get -y update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    python3-dev python3-pip g++ doxygen dvipng latexmk \
    cmake libjpeg8-dev zlib1g-dev texlive-latex-base \
    texlive-latex-extra git latex-cjk-all texlive-lang-all \
    graphviz python3-matplotlib wget unzip enchant-2 locales

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y python3-venv
ENV VIRTUAL_ENV=/opt/venv
RUN python3 -m venv $VIRTUAL_ENV
ENV PATH="$VIRTUAL_ENV/bin:$PATH"

RUN python3 -m pip install Sphinx breathe \
    sphinx_bootstrap_theme sphinxcontrib-bibtex \
    sphinx_rtd_theme recommonmark sphinx-markdown-tables \
    sphinxcontrib-spelling setuptools

# Set the locale for spelling
RUN sed -i -e 's/# en_GB.UTF-8 UTF-8/en_GB.UTF-8 UTF-8/' /etc/locale.gen && \
    dpkg-reconfigure --frontend=noninteractive locales && \
    update-locale LANG=en_GB.UTF-8

ENV LANG en_GB.UTF-8
