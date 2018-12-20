FROM ubuntu:18.04

RUN apt-get -y update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    python-dev python-pip g++ doxygen dvipng latexmk \
    cmake libjpeg8-dev zlib1g-dev texlive-latex-base \
    texlive-latex-extra git \
    graphviz python-matplotlib

RUN pip install Sphinx breathe \
    sphinx_bootstrap_theme awscli sphinxcontrib-bibtex \
    sphinx_rtd_theme recommonmark sphinx-markdown-tables


