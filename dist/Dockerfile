#
# RIOT Dockerfile
#
# the resulting image will contain everything needed to build RIOT.
#
# Setup: (only needed once per Dockerfile change)
# 1. install docker, add yourself to docker group, enable docker, relogin
# 2. # docker build -t riotbuild .
#
# Usage:
# 3. cd to riot root
# 4. # docker run -i -t -u $UID -v $(pwd):/data/riotbuild riotbuild ./dist/tools/compile_test/compile_test.py

FROM ubuntu

MAINTAINER Kaspar Schleiser <kaspar@schleiser.de>

ENV DEBIAN_FRONTEND noninteractive

RUN echo "deb http://ppa.launchpad.net/terry.guo/gcc-arm-embedded/ubuntu trusty main" > /etc/apt/sources.list.d/gcc-arm-embedded.list
RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-key FE324A81C208C89497EFC6246D1D8367A3421AFB

RUN apt-get update
RUN apt-get -y dist-upgrade

RUN apt-get -y install build-essential
RUN apt-get -y install git
RUN apt-get -y install gcc-multilib
RUN apt-get -y install gcc-arm-none-eabi
RUN apt-get -y install gcc-msp430
RUN apt-get -y install pcregrep libpcre3
RUN apt-get -y install qemu-system-x86 python3
RUN apt-get -y install g++-multilib
RUN apt-get -y install gcc-avr binutils-avr avr-libc
RUN apt-get -y install subversion curl wget python p7zip unzip

RUN mkdir -p /data/riotbuild
WORKDIR /data/riotbuild
