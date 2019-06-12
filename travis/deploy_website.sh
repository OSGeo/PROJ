#!/bin/bash

builddir=$1
destdir=$2

rm -rf $destdir/projdocs
mkdir -p $destdir/projdocs

cd $builddir/html
cp -rf * $destdir/projdocs
cp $builddir/latex/proj.pdf $destdir/projdocs

cd $destdir/projdocs
git init
git checkout -b gh-pages
git remote add origin git@github.com:OSGeo/PROJ.git

git config user.email "proj.bot@proj.bot"
git config user.name "PROJ deploybot"

# A few files we must manually create
echo "_site" > .gitignore
touch .nojekyll
echo "proj.org" > CNAME

git add -A
git commit -m "update with results of commit https://github.com/OSGeo/PROJ/commit/$TRAVIS_COMMIT"
git push -f origin gh-pages

