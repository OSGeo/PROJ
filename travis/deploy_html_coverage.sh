#!/bin/bash

# Not used currently. See discussion of https://github.com/OSGeo/proj.4/pull/1035

set -eu

mkdir proj-coverage
cd proj-coverage
git init
git config user.email "projbot@proj.bot"
git config user.name "PROJ coveragebot"
git config push.default matching
git remote add origin git@github.com:OSGeo/proj-coverage.git

cp -r ../coverage_html .
echo "Results of coverage of PROJ test suite" > README.md
echo "See http://rawgithub.com/OSGeo/proj-coverage/master/coverage_html/index.html" >> README.md
git add -A > /dev/null
git commit -m "update with results of commit https://github.com/OSGeo/proj.4/commit/$TRAVIS_COMMIT"
git push -f origin master

cd ..
