FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update \
    && apt upgrade -y \
    && apt install -y --no-install-recommends \
    apt-utils zip build-essential clang cmake libsdl2-dev libc6 libc-bin

WORKDIR /usr/src/solnce/
