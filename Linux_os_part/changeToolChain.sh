#!/bin/bash

export PATH=$PATH:$(pwd)/toolschain/gcc-arm-9.2-2019.12-x86_64-arm-none-linux-gnueabihf/bin

export CC=arm-none-linux-gnueabihf-gcc
export CXX=arm-none-linux-gnueabihf-g++
export AR=arm-none-linux-gnueabihf-ar
export RANLIB=arm-none-linux-gnueabihf-ranlib
