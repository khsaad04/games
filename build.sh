#!/usr/bin/env bash

set -xe

if [ ! -d "build" ]; then
    mkdir build
fi

cc -O3 -o build/breakout src/breakout.c -lraylib -lm
cc -O3 -o build/flappy_bird src/flappy_bird.c -lraylib -lm
cc -O3 -o build/snake src/snake.c -lraylib -lm
