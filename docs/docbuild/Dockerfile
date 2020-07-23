FROM ubuntu:18.04

RUN apt-get -y update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    python3-dev python3-pip g++ doxygen dvipng latexmk \
    cmake libjpeg8-dev zlib1g-dev texlive-latex-base \
    texlive-latex-extra git latex-cjk-all texlive-lang-all \
    graphviz python3-matplotlib wget unzip enchant

RUN pip3 install Sphinx breathe \
    sphinx_bootstrap_theme awscli sphinxcontrib-bibtex \
    sphinx_rtd_theme recommonmark sphinx-markdown-tables \
    sphinxcontrib-spelling


