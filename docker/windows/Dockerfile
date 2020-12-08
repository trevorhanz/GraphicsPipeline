FROM ubuntu:20.04

# ------------------------------------------------------
# Install build tools
# ------------------------------------------------------

RUN apt-get update -qq

RUN apt-get install -y cmake
RUN apt-get install -y gcc-mingw-w64
RUN apt-get install -y g++-mingw-w64
RUN apt-get install -y wget
RUN apt-get install -y zip

RUN apt-get clean

# ------------------------------------------------------
# Install glew
# ------------------------------------------------------

RUN cd /tmp
RUN wget https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0-win32.zip
RUN unzip glew-2.1.0-win32.zip
RUN cp -R glew-2.1.0/bin/Release/x64/*  /usr/x86_64-w64-mingw32/bin/
RUN cp -R glew-2.1.0/lib/Release/x64/*  /usr/x86_64-w64-mingw32/lib/
RUN cp -R glew-2.1.0/include/*          /usr/x86_64-w64-mingw32/include/
RUN rm -r glew-2.1.0
RUN rm glew-2.1.0-win32.zip
