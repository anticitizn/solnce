FROM ubuntu:20.04

RUN apt update \
    && DEBIAN_FRONTEND=noninteractive \
    && apt upgrade -y \
    && apt install -y --no-install-recommends \
    apt-utils build-essential clang cmake libsdl2-dev libc6 libc-bin

WORKDIR /usr/src/solnce/

CMD ["bash", "./build.sh"]
