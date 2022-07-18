FROM registry.gitlab.com/musicscience37/my-cpp-env-docker:clang14

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    texlive-base && \
    apt-get autoremove -y && \
    apt-get autoclean && \
    rm -r /var/lib/apt/lists/*
