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
git add man/man3/geodesic.3
git add man/man3/pj_init.3
git commit -m "Update man-pages from Sphinx-docs"
