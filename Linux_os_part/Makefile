#!/bin/bash

DIR_LIB_INC = adrvLibs/include
DIR_LIB_LINK = adrvLibs/lib
SPCSTART_DIR = spcstart_dir/
SPCSTARTWORK_DIR = spcstartwork_dir/
SPCSTART_OBJ_DIR = ${SPCSTART_DIR}obj/
SPCSTARTWORK_OBJ_DIR = ${SPCSTARTWORK_DIR}obj/
SPCSTART_INC_DIR = ${SPCSTART_DIR}inc
SPCSTARTWORK_INC_DIR = ${SPCSTARTWORK_DIR}inc
DIR_BIN = build/

SRC_SPCSTART = $(wildcard ${SPCSTART_DIR}src/*.c)
SRC_SPCSTARTWORK = $(wildcard ${SPCSTARTWORK_DIR}src/*.c)
OBJ_SPCSTART = $(patsubst %.c,${SPCSTART_OBJ_DIR}%.o,$(notdir ${SRC_SPCSTART}))
OBJ_SPCSTARTWORK = $(patsubst %.c,${SPCSTARTWORK_OBJ_DIR}%.o,$(notdir ${SRC_SPCSTARTWORK}))

all : build/spcstart build/spcstartwork

CROSS_COMPILE=arm-none-linux-gnueabihf-
CC=${CROSS_COMPILE}gcc
CFLAGS=-O3 -g3 -Wall -std=gnu99 -Wno-stringop-overflow -I${DIR_LIB_INC} -L${DIR_LIB_LINK}
LFLAGS= -lgpg-error -lgcrypt -lcrypto 

build/spcstart : ${OBJ_SPCSTART}
	$(CC) $(OBJ_SPCSTART) $(CFLAGS) -I${SPCSTART_INC_DIR} -I./ -o build/spcstart

${SPCSTART_OBJ_DIR}%.o:${SPCSTART_DIR}src/%.c
	$(CC) $(CFLAGS) -I${SPCSTART_INC_DIR} -I./ -c $< -o $@  

build/spcstartwork : ${OBJ_SPCSTARTWORK}
	$(CC) $(OBJ_SPCSTARTWORK) $(CFLAGS) -I${SPCSTARTWORK_INC_DIR} -I./ $(LFLAGS) -o build/spcstartwork

${SPCSTARTWORK_OBJ_DIR}%.o:${SPCSTARTWORK_DIR}src/%.c
	$(CC) $(CFLAGS) -I${SPCSTARTWORK_INC_DIR} -I./ $(LFLAGS) -c $< -o $@  

./PHONY:clean
clean:
	rm -f ${DIR_BIN}*
	rm -f ${SPCSTART_OBJ_DIR}*.o
	rm -f ${SPCSTARTWORK_OBJ_DIR}*.o


