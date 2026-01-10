FROM registry.gitlab.com/musicscience37projects/docker/my-cpp-env-docker:clang21

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    openssh-client \
    gnupg2 \
    vim \
    nano \
    git-lfs \
    sudo \
    # For icon conversion
    imagemagick \
    # For configuration of locale
    locales \
    && \
    apt-get autoremove -y && \
    apt-get autoclean && \
    rm -r /var/lib/apt/lists/* && \
    # Make ubuntu user able to use sudo command.
    echo ubuntu ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/ubuntu && \
    chmod 0440 /etc/sudoers.d/ubuntu && \
    # Allow use of pyenv from ubuntu user.
    chmod 0777 /root && \
    chmod -R 0777 /root/.pyenv && \
    # Configure locale
    sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen && \
    dpkg-reconfigure --frontend=noninteractive locales && \
    update-locale LANG=en_US.UTF-8
ENV LANG=en_US.UTF-8

# mimalloc
RUN git clone https://github.com/microsoft/mimalloc.git -b v3.0.10 && \
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
