# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line, and also
# from the environment for the first two.
SPHINXOPTS    ?=
HTMLOPTS      ?= -W
SPHINXBUILD   ?= sphinx-build
SOURCEDIR     = source
BUILDDIR      = build

ALLSPHINXOPTS   = -d $(BUILDDIR)/doctrees $(PAPEROPT_$(PAPER)) $(SPHINXOPTS) source

# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)/*
	rm -rf .doxygen_up_to_date

.doxygen_up_to_date:
	../scripts/doxygen.sh
	touch .doxygen_up_to_date

.PHONY: html
html: .doxygen_up_to_date
	$(SPHINXBUILD) -b html $(ALLSPHINXOPTS) $(HTMLOPTS) $(BUILDDIR)/html
	# Undoes the hack done in scripts/doxygen.sh
	@sed "s/io::Convention_/io::Convention/g" < $(BUILDDIR)/html/development/reference/cpp/io.html | sed "s/>Convention_/>Convention/g" | sed "s/_WKT2/WKT2/g" | sed "s/_WKT1/WKT1/g" > $(BUILDDIR)/html/development/reference/cpp/io.html.tmp
	@mv $(BUILDDIR)/html/development/reference/cpp/io.html.tmp $(BUILDDIR)/html/development/reference/cpp/io.html
	# Undoes the hacks of scripts/generate_breathe_friendly_general_doc.py
	@sed "s/<em class=\"property\">namespace <\/em>//g" < $(BUILDDIR)/html/development/reference/cpp/cpp_general.html > $(BUILDDIR)/html/development/reference/cpp/cpp_general.html.tmp
	@mv $(BUILDDIR)/html/development/reference/cpp/cpp_general.html.tmp $(BUILDDIR)/html/development/reference/cpp/cpp_general.html
	@cp -r ../schemas $(BUILDDIR)/html/schemas
	@echo
	@echo "Build finished. The HTML pages are in $(BUILDDIR)/html."


# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
