#!/usr/bin/env bash

set -xe

if [ ! -d "build" ]; then
    mkdir build
fi

clang -Wall -Wextra -o build/breakout src/breakout.c -lraylib -ggdb
clang -Wall -Wextra -o build/flappy_bird src/flappy_bird.c -lraylib -ggdb
clang -Wall -Wextra -o build/snake src/snake.c -lraylib -ggdb
