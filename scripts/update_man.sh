# run from root of repo
cd docs
make man
cd ..
cp -r docs/build/man/*.1 man/man1
git add man/man1/proj.1
git add man/man1/cs2cs.1
git add man/man1/cct.1
git add man/man1/geod.1
git add man/man1/gie.1
git add man/man1/projinfo.1
git add man/man1/projsync.1
git commit -m "Update man-pages from Sphinx-docs"
