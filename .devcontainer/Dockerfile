FROM registry.gitlab.com/musicscience37projects/docker/my-cpp-env-docker:clang17

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    openssh-client \
    gnupg2 \
    vim \
    nano \
    git-lfs \
    sudo \
    && \
    apt-get autoremove -y && \
    apt-get autoclean && \
    rm -r /var/lib/apt/lists/* && \
    # Make ubuntu user able to use sudo command.
    echo ubuntu ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/ubuntu && \
    chmod 0440 /etc/sudoers.d/ubuntu && \
    # Allow use of pyenv from ubuntu user.
    chmod 0777 /root && \
    chmod -R 0777 /root/.pyenv

# mimalloc
RUN git clone https://github.com/microsoft/mimalloc.git -b v2.1.2 && \
    cd mimalloc && \
    mkdir -p out/release && \
    cd out/release && \
    cmake -DCMAKE_BUILD_TYPE=Release ../.. && \
    cmake --build . && \
    cmake --build . --target install && \
    cd ../../.. && \
    rm -r mimalloc

# Prepare poetry for ubuntu user.
USER ubuntu
WORKDIR /home/ubuntu
ENV PATH="/home/ubuntu/.local/bin:$PATH"
RUN pipx install poetry
