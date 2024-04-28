#!/usr/bin/env bash

set -xe

if [ ! -d "build" ]; then
    mkdir build
fi

clang -O3 -Wall -Wextra -o build/breakout src/breakout.c -lraylib
clang -O3 -Wall -Wextra -o build/flappy_bird src/alappy_bird.c -lraylib
