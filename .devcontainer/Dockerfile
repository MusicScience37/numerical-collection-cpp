FROM registry.gitlab.com/musicscience37/my-cpp-env-docker:clang14

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    # For bibtex used in Doxygen.
    texlive-base \
    # For ps2pdf used in pprof.
    ghostscript \
    && \
    apt-get autoremove -y && \
    apt-get autoclean && \
    rm -r /var/lib/apt/lists/*
