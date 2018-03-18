# run from root of repo
cd docs
make man
cd ..
cp -r docs/build/man/*.1 man/man1
git add man/man1/*.1
git commit -m "Update man-pages from Sphinx-docs"
