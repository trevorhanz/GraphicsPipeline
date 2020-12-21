FROM centos:7

# ------------------------------------------------------
# Install build tools
# ------------------------------------------------------
RUN yum install -y wget gcc gcc-c++ make

# ------------------------------------------------------
# Install cmake3 from source
# ------------------------------------------------------
RUN wget https://cmake.org/files/v3.17/cmake-3.17.3.tar.gz && \
    tar -zxvf cmake-3.17.3.tar.gz && \
    rm cmake-3.17.3.tar.gz && \
    cd cmake-3.17.3 && \
    ./bootstrap --prefix=/usr/local -- -DCMAKE_USE_OPENSSL=OFF && \
    make -j install && \
    cd ../ && \
    rm -rf cmake-3.17.3 && \
    cd /usr/bin && \
    ln -s /usr/local/bin/cmake cmake3

# ------------------------------------------------------
# Install development packages
# ------------------------------------------------------
RUN yum install -y mesa-libGL-devel glew-devel
