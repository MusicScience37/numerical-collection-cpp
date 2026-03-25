FROM registry.gitlab.com/musicscience37projects/docker/my-cpp-env-docker:clang21

USER root
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    libvtk9-dev \
    && \
    apt-get autoremove -y && \
    apt-get autoclean && \
    rm -r /var/lib/apt/lists/*
