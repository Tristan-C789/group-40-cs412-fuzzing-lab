# Use AFL++ base image 
FROM aflplusplus/aflplusplus:latest

# Install wget & zlib
RUN apt-get update && apt-get install -y wget zlib1g-dev patch

# Make home directory
WORKDIR /fuzz

# Download libpng source
RUN wget -q https://download.sourceforge.net/libpng/libpng-1.6.18.tar.gz && \
    tar xf libpng-1.6.18.tar.gz && \
    rm libpng-1.6.18.tar.gz

# Patch libpng 
COPY patches/ /fuzz/patches/
RUN cd libpng-1.6.18 && patch -p0 < /fuzz/patches/nocrc.patch

# Compile libpng with instrumentalisation 
RUN cd libpng-1.6.18 && \
    CC=afl-clang-fast \
    CXX=afl-clang-fast++ \
    CFLAGS="-fsanitize=address -g -O1" \
    LDFLAGS="-fsanitize=address" \
    ./configure --disable-shared --prefix=/fuzz/install && \
    make -j$(nproc) && \
    make install

# Compile libpng without instrumentalisation
RUN cd libpng-1.6.18 && \
    make distclean && \
    CC=gcc \
    CFLAGS="-g -O1" \
    ./configure --disable-shared --prefix=/fuzz/install_vanilla && \
    make -j$(nproc) && \
    make install

# Copy dependencies
COPY src/ /fuzz/src/
COPY seeds/ /fuzz/seeds/
COPY Makefile /fuzz/Makefile

# Copy AFL++ seeds
RUN cp /AFLplusplus/testcases/images/png/*.png /fuzz/seeds/

# Make output directories
RUN mkdir -p findings/default
RUN mkdir -p findings-qemu/default
RUN mkdir -p plot_output
RUN mkdir -p plot_output_qemu

# Launch shell
CMD ["/bin/bash"]