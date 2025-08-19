#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#include <stdbool.h>
#include <string.h>

#define BUILD_PATH "build/"
#define SRC_PATH "src/"
#define RAYLIB_PATH "raylib-5.5_linux_amd64/"

typedef enum { DEBUG, RELEASE } BuildType;

bool build_game(Cmd *cmd, char *name, BuildType build_type)
{
    cmd_append(cmd, "cc");

    switch (build_type) {
    case DEBUG:
        cmd_append(cmd, "-Wall", "-Wextra", "-ggdb");
        break;
    case RELEASE:
        cmd_append(cmd, "-O3");
        break;
    }

    cmd_append(cmd, "-I./"RAYLIB_PATH"include",
               "-o", temp_sprintf("%s%s", BUILD_PATH, name),
               temp_sprintf("%s%s.c", SRC_PATH, name),
               "-L./"RAYLIB_PATH"lib",
               "-l:libraylib.a", "-lm");

    if (!cmd_run_sync_and_reset(cmd)) return false;
    return true;
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    if (!mkdir_if_not_exists(BUILD_PATH)) return 1;

    Cmd cmd = {0};
    BuildType build_type = RELEASE;

    shift(argv, argc);
    if (argc > 0) {
        const char *arg = shift(argv, argc);
        if (strcmp(arg, "-debug") == 0) {
            build_type = DEBUG;
        } else {
            nob_log(NOB_ERROR, "invalid argument: %s", arg);
            return 1;
        }
    }

    if (!build_game(&cmd, "breakout",    build_type)) return 1;
    if (!build_game(&cmd, "flappy_bird", build_type)) return 1;
    if (!build_game(&cmd, "snake",       build_type)) return 1;

    return 0;
}
