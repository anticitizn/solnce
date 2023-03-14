FROM ubuntu:22.10

RUN apt update \
    && apt upgrade -y \
    && apt install -y \
    apt-utils build-essential clang cmake libsdl2-dev

WORKDIR /usr/src/solnce/

CMD ["bash", "./build.sh"]
