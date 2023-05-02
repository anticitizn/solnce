FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update \
    && apt install -y --no-install-recommends \
    apt-utils zip build-essential cmake mingw-w64

#RUN mkdir /usr/local/x86_64-w64-mingw32

WORKDIR /usr/src/solnce/external/sdl
COPY external/sdl ./

RUN make native

WORKDIR /usr/src/solnce/