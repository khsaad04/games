#!/usr/bin/env bash

set -xe

if [ ! -d "bin" ]; then
    mkdir bin
fi

cc -Wall -Wextra -Wpedantic -ggdb -o bin/breakout src/breakout.c -lraylib -lm
cc -Wall -Wextra -Wpedantic -ggdb -o bin/flappy_bird src/flappy_bird.c -lraylib -lm
cc -Wall -Wextra -Wpedantic -ggdb -o bin/snake src/snake.c -lraylib -lm
