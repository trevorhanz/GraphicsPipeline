FROM ubuntu:20.04

# ------------------------------------------------------
# Install build tools
# ------------------------------------------------------

RUN apt-get update -qq && \
    apt-get install -y cmake gcc-mingw-w64 g++-mingw-w64 wget zip && \
    apt-get clean

# ------------------------------------------------------
# Install glew
# ------------------------------------------------------

RUN wget https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0-win32.zip && \
    unzip glew-2.1.0-win32.zip && \
    cp -R glew-2.1.0/bin/Release/x64/*  /usr/x86_64-w64-mingw32/bin/ && \
    cp -R glew-2.1.0/lib/Release/x64/*  /usr/x86_64-w64-mingw32/lib/ && \
    cp -R glew-2.1.0/include/*          /usr/x86_64-w64-mingw32/include/ && \
    rm -r glew-2.1.0 && \
    rm glew-2.1.0-win32.zip
