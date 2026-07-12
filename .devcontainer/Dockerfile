FROM registry.gitlab.com/musicscience37projects/docker/my-cpp-env-docker:clang22

USER root
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    libvtk9-dev \
    # libvtk9-dev depends on libstdc++-16, but libstdc++-16-dev is not installed by default, and cause link errors.
    libstdc++-16-dev \
    freefem++ libfreefem++ \
    && \
    apt-get autoremove -y && \
    apt-get autoclean && \
    rm -r /var/lib/apt/lists/* && \
    # FreeFEM searches libraries in /usr/lib/ff++/4.15/lib, but actually they are in /usr/lib/freefem++.
    # Apt packages seem to be broken, so workaround the problem by creating a symbolic link.
    mkdir -p /usr/lib/ff++/4.15 && \
    ln -s /usr/lib/freefem++ /usr/lib/ff++/4.15/lib
