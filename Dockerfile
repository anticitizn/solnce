FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update \
    && apt upgrade -y \
    && apt install -y --no-install-recommends \
    apt-utils zip build-essential cmake gcc-mingw-w64 g++-mingw-w64

RUN mkdir /usr/local/x86_64-w64-mingw32

WORKDIR /usr/src/solnce/external/sdl
COPY external/sdl ./
RUN mkdir build
WORKDIR /usr/src/solnce/external/sdl/build

RUN make cross
RUN make install-package

WORKDIR /usr/src/solnce/