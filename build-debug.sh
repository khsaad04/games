#!/usr/bin/env bash

set -xe

if [ ! -d "bin" ]; then
    mkdir bin
fi

cc -Wall -Wextra -pedantic -o bin/breakout src/breakout.c -lraylib -lm -ggdb
cc -Wall -Wextra -pedantic -o bin/flappy_bird src/flappy_bird.c -lraylib -lm -ggdb
cc -Wall -Wextra -pedantic -o bin/snake src/snake.c -lraylib -lm -ggdb
