# Stage 1: Install System Dependencies
FROM ubuntu:latest AS dependencies

# Install essential build tools and utilities
RUN apt-get update && apt-get install -y \
    build-essential cmake git doxygen python3 python3-pip ninja-build \
    graphviz pkg-config dos2unix cmake-format libssl-dev clang-tidy clang-format \
    curl zip unzip tar && \
    rm -rf /var/lib/apt/lists/*

# Stage 2: Install vcpkg and Dependencies
FROM dependencies AS vcpkg_build

# Clone and bootstrap vcpkg
RUN git clone --depth 1 https://github.com/microsoft/vcpkg.git /usr/local/vcpkg && \
    sh /usr/local/vcpkg/bootstrap-vcpkg.sh

# Copy vcpkg configuration files
COPY vcpkg.json vcpkg-configuration.json /

# Install dependencies using vcpkg
WORKDIR /

RUN /usr/local/vcpkg/vcpkg install --triplet x64-linux-release --clean-after-build

RUN rm vcpkg.json vcpkg-configuration.json
    
RUN zip -r -9 vcpkg_installed.zip /vcpkg_installed && rm -rf /vcpkg_installed
