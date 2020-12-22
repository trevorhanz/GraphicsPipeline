FROM centos:8

# ------------------------------------------------------
# Install build tools
# ------------------------------------------------------
RUN yum install -y wget gcc gcc-c++ make cmake

# ------------------------------------------------------
# Install development packages
# ------------------------------------------------------
RUN yum install -y mesa-libGL-devel mesa-libGLU-devel

# Install glew
RUN wget https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0.tgz && \
    tar xvf glew-2.1.0.tgz && \
    make -j install -C /glew-2.1.0 && \
    rm glew-2.1.0.tgz && \
    rm -rf glew-2.1.0
