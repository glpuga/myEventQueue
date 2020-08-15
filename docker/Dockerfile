FROM ubuntu:18.04

# Setup environment
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get install -y \
	sudo \
	debian-keyring \
	debian-archive-keyring

RUN apt-get update && apt-get install -y \
	git \
	locate \
	mc

RUN apt-get update && apt-get install -y \
	build-essential \
	cmake 

RUN apt-get update && apt-get install -y \
	clang \
	clang-format \
	clang-tidy \
	python3-pip \
	cppcheck

RUN pip3 install cpplint

RUN pip3 install cmake-format

RUN mkdir /workspace

WORKDIR /workspace
CMD /bin/bash
