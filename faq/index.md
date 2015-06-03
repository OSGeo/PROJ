---
layout: default
title: Frequently Asked Questions
description: Frequently asked questions
keywords: faq, questions, problems, solutions
---

# FAQ

[Where can I find the list of projections and their arguments?](where-can-i-find-the-list-of-projections-and-their-arguments)

Where can I find the list of projections and their arguments?
-------------------------------------------------------------

There is no simple single location to find all the required information. The PostScript/PDF documents listed on the .main PROJ.4 page under documentation are the authoritative source but projections and options are spread over several documents in a form more related to their order of implementation than anything else.

The `proj` command itself can report the list of projections using the `-lp` option, the list of ellipsoids with the `-le` option, the list of units with the `-lu` option, and the list of built-in datums with the `-ld` option.

The [GeoTIFF Projections Pages](http://www.remotesensing.org/geotiff/proj_list/) include most of the common PROJ.4 projections, and a definition of the projection specific options for each. 
