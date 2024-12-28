#!/usr/bin/env bash

set -xe

if [ ! -d "bin" ]; then
    mkdir bin
fi

cc -O3 -o bin/breakout src/breakout.c -lraylib -lm
cc -O3 -o bin/flappy_bird src/flappy_bird.c -lraylib -lm
cc -O3 -o bin/snake src/snake.c -lraylib -lm
