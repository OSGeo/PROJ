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
user. Familiarity with [issues](https://guides.github.com/features/issues/) and
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
describes a new projection.

Note that not all feature requests are accepted.

## Write documentation

PROJ.4 is in dire need of better documentation. Any contributiions of documentation
are greatly appreaciated. The PROJ.4 documentation is available on [proj4.org](ttp://proj4.org).
The website is generated with [Sphinx](http://www.sphinx-doc.org/en/stable/). Contributions
to the documentation should be made as [Pull Requests](https://github.com/OSGeo/proj.4/pulls)
on GitHub.

If you intend to document on of PROJ.4's supported projections please use the
[Mercator projection](http://proj4.org/projections/merc.html) as a template.


## Code contributions

Code contributions can be either bug fixes or new features. The process is the same
for both, so they will be discussed together in this section.

### Making Changes

* Create a topic branch from where you want to base your work.
  * You usually should base your topic branch off of the master branch.
  * To quickly create a topic branch: `git checkout -b my-topic-branch`
* Make commits of logical units.
* Check for unnecessary whitespace with `git diff --check` before committing.
* Make sure your commit messages are in the [proper format](http://tbaggery.com/2008/04/19/a-note-about-git-commit-messages.html).
* Make sure you have added the necessary tests for your changes.
* Make sure that all tests pass

### Submitting Changes

* Push your changes to a topic branch in your fork of the repository.
* Submit a pull request to the PROJ.4 repository in the OSGeo organization.
  * If your pull request fixes/references an issue, include that issue number in the pull request. For example:

```
Wiz the bang

Fixes #123.
```

* PROJ.4 developers will look at your patch and take an appropriate action.

### Coding conventions

#### Programming language
PROJ.4 is developed strictly in ANSI C.

#### Coding style
We don't enforce any particular coding style, but please try to keep it as simple as possible.

#### Whitespace
Throughout the PROJ.4 code base you will see differing whitespace use.
The general rule is to keep whitespace in whatever form it is
in the file you are currently editing. If the file has a mix of tabs and space please
convert the tabs to space in a separate commit before making any other changes. This
makes it a lot easier to see the changes in diffs when evaulating the changed code. New
files should use spaces as whitespace.

#### File names
Files in which projections are implemented are prefixed with an upper-case `PJ_` and most other
files are prefixed with lower-case `pj_`. Some file deviate from this pattern, most of them dates
back to the very early releases of PROJ.4. New contributions should follow the pj-prefix pattern.
Unless there are obvious reasons not to.

## Additional Resources

* [General GitHub documentation](http://help.github.com/)
* [GitHub pull request documentation](http://help.github.com/send-pull-requests/)

## Acknowledgements

The _code contribution_ sectino of this CONTRIBUTING file is inspired by[PDAL's](https://github.com/PDAL/PDAL/blob/master/CONTRIBUTING.md).
