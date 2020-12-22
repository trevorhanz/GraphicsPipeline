FROM ubuntu:20.04

ENV ANDROID_NDK_HOME /opt/android-ndk


# ------------------------------------------------------
# Install required tools
# ------------------------------------------------------

RUN apt-get update -qq && \
    apt install -y cmake wget unzip && \
    apt-get clean

# ------------------------------------------------------
# Android NDK
# ------------------------------------------------------

# download
RUN mkdir /opt/android-ndk-tmp && \
    cd /opt/android-ndk-tmp && \
    wget -q http://dl.google.com/android/repository/android-ndk-r21d-linux-x86_64.zip && \
    unzip ./android-ndk-r21d-linux-x86_64.zip && \
    rm ./android-ndk-r21d-linux-x86_64.zip && \
    mv ./android-ndk-r21d /opt/android-ndk && \
    rm -rf /opt/android-ndk-tmp

# add to PATH
ENV PATH ${PATH}:${ANDROID_NDK_HOME}

# ------------------------------------------------------
# Cleanup and rev num
# ------------------------------------------------------

CMD bitrise -version
