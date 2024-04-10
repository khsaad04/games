#!/usr/bin/env bash

set -xe

if [ ! -d "build" ]; then
    mkdir build
fi

cc -Wall -Wextra -o build/breakout breakout.c -lraylib
cc -Wall -Wextra -o build/flappy_bird flappy_bird.c -lraylib
