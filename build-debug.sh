#!/usr/bin/env bash

set -xe

if [ ! -d "build" ]; then
    mkdir build
fi

cc -Wall -Wextra -pedantic -o build/breakout src/breakout.c -lraylib -ggdb
cc -Wall -Wextra -pedantic -o build/flappy_bird src/flappy_bird.c -lraylib -ggdb
cc -Wall -Wextra -pedantic -o build/snake src/snake.c -lraylib -ggdb
