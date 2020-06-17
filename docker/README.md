# Docker Container
All special containers are generated form the following docker files.

# Building
To build a docker container:
```bash
$ docker build -f <docker file> -t <name[:tag]> .
```

examples:
```bash
$ docker build -f emscripten/Dockerfile -t trevorhanz/emscripten:1.39.4 .
```
