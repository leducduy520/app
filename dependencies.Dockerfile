# Stage 1: Install System Dependencies
FROM ubuntu:latest AS dependencies

# Install essential build tools and utilities
RUN apt-get update
RUN apt-get install -y --no-install-recommends build-essential cmake git
RUN apt-get install -y --no-install-recommends curl zip unzip tar
RUN apt-get install -y --no-install-recommends ca-certificates
RUN apt-get install -y --no-install-recommends pkg-config
RUN rm -rf /var/lib/apt/lists/*
    

# Stage 2: Install vcpkg and Dependencies
FROM dependencies AS vcpkg_build

# Clone and bootstrap vcpkg
RUN git clone --depth 1 https://github.com/microsoft/vcpkg.git /vcpkg && \
    sh /vcpkg/bootstrap-vcpkg.sh

# Copy vcpkg configuration files
COPY vcpkg.json vcpkg-configuration.json /

# Install dependencies using vcpkg
WORKDIR /

RUN vcpkg/vcpkg install --triplet x64-linux-release --clean-after-build

RUN rm vcpkg.json vcpkg-configuration.json
    
RUN zip -r -9 vcpkg_installed.zip /vcpkg_installed && rm -rf /vcpkg_installed
