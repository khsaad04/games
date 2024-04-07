#!/usr/bin/env bash
if [ ! -d "build" ]; then
    mkdir build
fi
cc -Wall -Wextra -o build/breakout breakout.c -lraylib
