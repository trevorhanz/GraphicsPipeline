FROM ubuntu:20.04

# ------------------------------------------------------
# Install required tools
# ------------------------------------------------------

RUN apt-get update -qq && \
    apt install -y wget python3 cmake && \
    apt install -y doxygen graphviz cpio && \
    apt-get clean

RUN cd /opt/ && \
    wget https://github.com/emscripten-core/emsdk/archive/2.0.12.tar.gz && \
    tar -xvf 2.0.12.tar.gz && \
    rm 2.0.12.tar.gz && \
    cd emsdk-2.0.12 && \
    ./emsdk install 2.0.12 && \
    ./emsdk activate 2.0.12

# ------------------------------------------------------
# Setup environment
# ------------------------------------------------------

ENV PATH="/opt/emsdk-2.0.12:/opt/emsdk-2.0.12/node/12.18.1_64bit/bin:/opt/emsdk-2.0.12/upstream/emscripten:${PATH}"
ENV EMSCRIPTEN="/opt/emsdk-2.0.12/upstream/emscripten/"
