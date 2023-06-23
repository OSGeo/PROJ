This directory contain fuzzer main functions and scripts for the
Google OSS Fuzz project: https://github.com/google/oss-fuzz/

The main build scripts are in:
https://github.com/google/oss-fuzz/tree/master/projects/proj4
and call scripts in this directory.

The list of issues is in:
https://bugs.chromium.org/p/oss-fuzz/issues/list?q=proj4

- Build proj_crs_to_crs_fuzzer in a standalone mode with the
  -DBUILD_TESTING=ON CMake options

  Run it:
    $ bin/proj_crs_to_crs_fuzzer {file_generated_by_oss_fuzz}

- Run locally OSS Fuzz:
    $ git clone https://github.com/google/oss-fuzz.git
    $ cd oss-fuzz
    $ export PROJECT_NAME=proj4
    $ python infra/helper.py build_image $PROJECT_NAME

  Build fuzzers with the address sanitizer (could use undefined, etc...)
    $ python infra/helper.py build_fuzzers --sanitizer address $PROJECT_NAME

  Test a particular fuzzer (replace proj_crs_to_crs_fuzzer by other fuzzers)
    $ python infra/helper.py run_fuzzer $PROJECT_NAME proj_crs_to_crs_fuzzer


How to deal with issues reported in https://bugs.chromium.org/p/oss-fuzz/issues/list?q=proj4 ?

    1. Leave a comment in (chromium database) bug entry to indicate that you work on it
    2. Work
    3. Commit a bug fix with log including "Credit to OSS-Fuzz" and a link to the bugs.chromium.org ticket
    4. Add in the bugs.chromium.org ticket a link to the github changeset implementing the fix.
    5. Check chromium closed the bug (after one or two days typically)
