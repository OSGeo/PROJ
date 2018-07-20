# How to contribute to PROJ.4

PROJ.4 has a wide and varied user base. Some are highly skilled geodesists with a deep
knowledge of map projections and reference systems, some are GIS software developers
and others are GIS users. All users, regardless of the profession or skill level,
has the ability to contribute to PROJ.4. Here's a few suggestion on how:

* Help PROJ.4-users that is less experienced than yourself.
* Write a bug report
* Request a new feature
* Write documentation for your favorite map projection
* Fix a bug
* Implement a new feature

In the following sections you can find some guidelines on how to contribute.
As PROJ.4 is managed on GitHub most contributions require that you have a GitHub
account. Familiarity with [issues](https://guides.github.com/features/issues/) and
the [GitHub Flow](https://guides.github.com/introduction/flow/) is an advantage.

## Help a fellow PROJ.4 user
The main forum for support for PROJ.4 is the mailing list. You can subscribe to
the mailing list [here](http://lists.maptools.org/mailman/listinfo/proj) and read
the archive [here](http://lists.maptools.org/pipermail/proj/).

If you have questions about the usage of PROJ.4 the mailing list is also the place to go.
Please *do not* use the GitHub issue tracker as a support forum. Your question is much
more likely to be answered on the mailing list, as many more people follow that than
the issue tracker.

## Adding bug reports

Bug reports are handled in the [issue tracker](https://github.com/OSGeo/proj.4/issues)
on PROJ.4's home on GitHub. Writing a good bug report is not easy. But fixing a poorly
documented bug is not easy either, so please put in the effort it takes to create a
thorough bug report.

A good bug report includes at least:

* A title that quickly explains the problem
* A description of the problem and how it can be reproduced
* Version of PROJ.4 being used
* Version numbers of any other relevant software being used, e.g. operating system
* A description of what already has been done to solve the problem

The more information that is given up front, the more likely it is that a developer
will find interest in solving the problem. You will probably get follow-up questions
after submitting a bug report. Please answer them in a timely manner if you have an
interest in getting the issue solved.

Finally, please only submit bug reports that are actually related to PROJ.4. If the
issue materializes in software that uses PROJ.4 it is likely a problem with that
particular software. Make sure that it actually is a PROJ.4 problem before you submit
an issue. If you can reproduce the problem only by using tools from PROJ.4 it is
definitely a problem with PROJ.4.

## Feature requests

Got an idea for a new feature in PROJ.4? Submit a thorough description of the new
feature in the [issue tracker](https://github.com/OSGeo/proj.4/issues). Please
include any technical documents that can help the developer make the new feature a
reality. An example of this could be a publicly available academic paper that
describes a new projection. Also, including a numerical test case will make it
much easier to verify that an implementation of your requested feature actually
works as you expect.

Note that not all feature requests are accepted.

## Write documentation

PROJ.4 is in dire need of better documentation. Any contributions of documentation
are greatly appreciated. The PROJ.4 documentation is available on [proj4.org](https://proj4.org).
The website is generated with [Sphinx](http://www.sphinx-doc.org/en/stable/). Contributions
to the documentation should be made as [Pull Requests](https://github.com/OSGeo/proj.4/pulls)
on GitHub.

If you intend to document one of PROJ.4's supported projections please use the
[Mercator projection](https://proj4.org/projections/merc.html) as a template.


## Code contributions

See [Code Contributions](https://proj4.org/development/for_proj_contributors.html)

#### Legalese
Committers are the front line gatekeepers to keep the code base clear of improperly contributed code.
It is important to the PROJ.4 users, developers and the OSGeo foundation to avoid contributing any
code to the project without it being clearly licensed under the project license.

Generally speaking the key issues are that those providing code to be included in the repository
understand that the code will be released under the MIT/X license, and that the person providing
the code has the right to contribute the code. For the committer themselves understanding about
the license is hopefully clear. For other contributors, the committer should verify the understanding
unless the committer is very comfortable that the contributor understands the license (for
instance frequent contributors).

If the contribution was developed on behalf of an employer (on work time, as part of a work project,
etc) then it is important that an appropriate representative of the employer understand that the
code will be contributed under the MIT/X license. The arrangement should be cleared with an
authorized supervisor/manager, etc.

The code should be developed by the contributor, or the code should be from a source which can be
rightfully contributed such as from the public domain, or from an open source project under a
compatible license.

All unusual situations need to be discussed and/or documented.

Committer should adhere to the following guidelines, and may be personally legally liable for
improperly contributing code to the source repository:

* Make sure the contributor (and possibly employer) is aware of the contribution terms.
* Code coming from a source other than the contributor (such as adapted from another project)
  should be clearly marked as to the original source, copyright holders, license terms and so forth.
  This information can be in the file headers, but should also be added to the project licensing
  file if not exactly matching normal project licensing (COPYING).
* Existing copyright headers and license text should never be stripped from a file. If a copyright
  holder wishes to give up copyright they must do so in writing to the foundation before copyright
  messages are removed. If license terms are changed it has to be by agreement (written in email is
  ok) of the copyright holders.
* Code with licenses requiring credit, or disclosure to users should be added to COPYING.
* When substantial contributions are added to a file (such as substantial patches) the
  author/contributor should be added to the list of copyright holders for the file.
* If there is uncertainty about whether a change is proper to contribute to the code base, please
  seek more information from the project steering committee, or the foundation legal counsel.


## Additional Resources

* [General GitHub documentation](https://help.github.com/)
* [GitHub pull request documentation](https://help.github.com/articles/about-pull-requests/)

## Acknowledgements

The _code contribution_ section of this CONTRIBUTING file is inspired by
[PDAL's](https://github.com/PDAL/PDAL/blob/master/CONTRIBUTING.md) and the _legalese_ section is
modified from [GDAL committer guidelines](https://trac.osgeo.org/gdal/wiki/rfc3_commiters)
