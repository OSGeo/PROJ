FROM ubuntu:18.04

RUN apt-get -y update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    python3-dev python3-pip g++ doxygen dvipng latexmk \
    cmake libjpeg8-dev zlib1g-dev texlive-latex-base \
    texlive-latex-extra git latex-cjk-all texlive-lang-all \
    graphviz python3-matplotlib wget unzip enchant locales

RUN python3 -m pip install Sphinx breathe \
    sphinx_bootstrap_theme awscli sphinxcontrib-bibtex \
    sphinx_rtd_theme recommonmark sphinx-markdown-tables \
    sphinxcontrib-spelling

# Set the locale for spelling
RUN sed -i -e 's/# en_GB.UTF-8 UTF-8/en_GB.UTF-8 UTF-8/' /etc/locale.gen && \
    dpkg-reconfigure --frontend=noninteractive locales && \
    update-locale LANG=en_GB.UTF-8

ENV LANG en_GB.UTF-8
