# Best OS ^_^
FROM archlinux:latest

# Install dependencies
RUN pacman -Syu --noconfirm && \
    pacman -S --noconfirm base-devel cmake protobuf libpqxx openssl-1.1 boost numactl

ENV OPENSSL_ROOT_DIR=/usr/lib/openssl-1.1
ENV OPENSSL_LIBRARIES=/usr/lib/openssl-1.1


# Verify OpenSSL version
RUN openssl version -a

# Set working directory
WORKDIR /app

# Copy source files
COPY . .

# Navigate to the tests directory
WORKDIR /app/tests

# Clean up existing build directory (if necessary)
RUN rm -rf build

# Create a new build directory
RUN mkdir build

# Navigate to the build directory
WORKDIR /app/tests/build

# Run cmake to generate build files
RUN cmake -DOPENSSL_ROOT_DIR=/usr -DOPENSSL_LIBRARIES=/usr/lib ..

# Build the server
RUN make

# Expose the port (if applicable)
EXPOSE 6121

# Set the default command to run your server
CMD ["./server"]

#To run docker container on your VPS, you can use the following command:
#docker run -d --network host <image_name>
