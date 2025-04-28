FROM mcr.microsoft.com/devcontainers/cpp:1-debian-11

# Install additional packages
RUN apt-get update && apt-get install -y \
    openssh-client \
    rsync \
    gdb \
    wget \
    autoconf \
    automake \
    libtool \
    make \
    gcc \
    g++ \
    gcc-arm-linux-gnueabihf \
    && apt-get clean && rm -rf /var/lib/apt/lists/*
