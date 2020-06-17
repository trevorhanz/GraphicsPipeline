FROM trzeci/emscripten:1.39.4-ubuntu

# ------------------------------------------------------
# Install required tools
# ------------------------------------------------------

RUN apt-get update -qq

RUN apt install -y doxygen

# ------------------------------------------------------
# Cleanup and rev num
# ------------------------------------------------------

# Cleaning
RUN apt-get clean
