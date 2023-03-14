FROM ubuntu:22.10

RUN apt update \
    && apt upgrade -y \
    && apt install -y \
    apt-utils build-essential clang cmake libsdl2-2.0-
    
COPY . /usrc/src/solnce

WORKDIR /usr/src/solnce

CMD ["bash", "./build.sh"]