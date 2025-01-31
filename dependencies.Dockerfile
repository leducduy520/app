# Stage 1: Install System Dependencies
FROM ubuntu:latest AS dependencies

# Install essential build tools and utilities
RUN apt-get update
RUN apt-get install -y --no-install-recommends build-essential cmake git
RUN apt-get install -y --no-install-recommends curl zip unzip tar
RUN apt-get install -y --no-install-recommends ca-certificates
RUN apt-get install -y --no-install-recommends pkg-config

# Stage 2: Install vcpkg and Dependencies
FROM dependencies AS vcpkg_build

# Clone and bootstrap vcpkg
RUN git clone --depth 1 https://github.com/microsoft/vcpkg.git /vcpkg && \
    sh /vcpkg/bootstrap-vcpkg.sh

# Copy vcpkg configuration files
COPY vcpkg.json vcpkg-configuration.json /

# Install dependencies using vcpkg
WORKDIR /

RUN apt-get install -y --no-install-recommends wget
RUN wget https://archives.boost.io/release/1.86.0/source/boost_1_86_0.tar.gz
RUN tar -xzf boost_1_86_0.tar.gz

WORKDIR /boost_1_86_0

RUN ./bootstrap.sh
RUN ./b2 install --with-random --with-system --with-thread --with-filesystem --with-chrono --with-atomic --with-date_time --with-regex
RUN apt-get install -y --no-install-recommends libcpprest-dev

WORKDIR /

RUN vcpkg/vcpkg install --triplet x64-linux-release --clean-after-build --x-install-root=/vcpkg/vcpkg_installed
RUN rm vcpkg.json vcpkg-configuration.json

RUN rm boost_1_86_0.tar.gz
RUN rm -rf /var/lib/apt/lists/*