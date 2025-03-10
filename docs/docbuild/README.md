# Building PROJ documentation with proj-docs docker image

From the base PROJ repository directory:

```bash
cd docs
docker run -i -t -v $PWD/..:/io -w /io/docs/ ghcr.io/osgeo/proj-docs make html
```

When a projection image is added or deleted run

```bash
python3 docs/source/operations/projections/generate_all_images.py
```
