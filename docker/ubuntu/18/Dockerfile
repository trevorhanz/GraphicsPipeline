FROM ubuntu:18.04

# ------------------------------------------------------
# Install build tools
# ------------------------------------------------------
RUN apt-get update -qq && \
    apt install -y gcc g++ make cmake && \
    apt install -y libgl1-mesa-dev libglew-dev && \
    apt-get clean
