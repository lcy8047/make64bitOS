FROM ubuntu:20.04

RUN dpkg --add-architecture i386
RUN apt-get -y update
RUN apt-get -y upgrade --fix-missing

RUN apt-get -y install libc6:i386 libncurses5:i386 libstdc++6:i386
RUN apt-get -y install build-essential vim gcc gcc-multilib nasm qemu