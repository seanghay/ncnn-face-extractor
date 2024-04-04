## Example Usage

- `photos/**/*.jpg` Required

```shell
 docker run -it --rm \
  -v $PWD/photos:/workspace/build/photos \
  -v $PWD/results:/workspace/build/results \
  ghcr.io/seanghay/ncnn-face-extractor:main
```
