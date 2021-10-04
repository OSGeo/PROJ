# Building PROJ documentation with proj-docs docker image

From the base PROJ repository directory:

```bash
cd docs
docker run -i -t -v $PWD/..:/io -w /io/docs/ osgeo/proj-docs make html
```
