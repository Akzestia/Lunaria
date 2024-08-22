# Best OS ^_^
FROM archlinux:latest

# Install dependencies
RUN pacman -Syu --noconfirm && \
    pacman -S --noconfirm cmake \
    protobuf \
    libpqxx \
    openssl-1.1 \
    boost numactl \
    libevent \
    git \
    go \
    ninja \
    curl \
    make \
    gcc \
    libcrypto.so \
    abseil-cpp

# Create a non-root user
RUN useradd -m builduser && echo "builduser ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

# Switch to the non-root user
USER builduser

# Download the pre-built release tarball of cassandra-cpp-driver
WORKDIR /tmp
RUN curl -L https://github.com/datastax/cpp-driver/archive/refs/tags/2.16.1.tar.gz -o cpp-driver-2.16.1.tar.gz

# Extract the tarball
RUN tar -xzf cpp-driver-2.16.1.tar.gz

# Set working directory to the extracted source
WORKDIR /tmp/cpp-driver-2.16.1

# Build the cassandra-cpp-driver using Ninja
RUN mkdir build && cd build && cmake -DCMAKE_INSTALL_PREFIX=/usr -G Ninja -DCMAKE_CXX_FLAGS="-Wno-error=deprecated-declarations" .. && ninja

# Switch to root to install the driver
USER root
RUN cd /tmp/cpp-driver-2.16.1/build && ninja install

# Clean up temporary files
RUN rm -rf /tmp/cpp-driver-2.16.1 /tmp/cpp-driver-2.16.1.tar.gz

# Set the working directory for your application
WORKDIR /app

# Copy your application files into the container
COPY . .

# Make sure the compile script is executable and run it
RUN chmod +x ./compile.proto.sh
RUN ./compile.proto.sh

# Navigate to the tests directory
WORKDIR /app/tests

# Clean up existing build directory (if necessary)
RUN rm -rf build

# Create a new build directory
RUN mkdir build
WORKDIR /app/tests/build

# Run cmake to generate build files and build the server
RUN cmake -DUSE_SCYLLA_DB=ON -G Ninja ..
RUN ninja

# Expose the port (if applicable)
EXPOSE 6121

# Set the default command to run your server
CMD ["./server"]

#To run docker container on your VPS, you can use the following command:
#docker run -d --network host <image_name>
