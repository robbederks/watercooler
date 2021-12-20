#!/bin/bash -e

TOOLS_PATH=/opt/microchip/xc8/v2.32/bin
TOOL_PREFIX=xc8
TOOL=$TOOLS_PATH/$TOOL_PREFIX

OUT_DIR=out/
mkdir -p $OUT_DIR

rm -r $OUT_DIR/*

$TOOL-cc \
    -mcpu=pic18f6527 \
    -opt=none \
    -mram=default,-0000-0100 \
    -o out/hook \
    hook.c

