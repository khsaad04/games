#include <stdbool.h>
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"
#define RAYLIB_FOLDER "raylib-5.5_linux_amd64/"

bool build_game(Cmd *cmd, char *name)
{
    cmd_append(cmd,
        "cc",
        "-Wall", "-Wextra", "-ggdb", "-O3",
        "-I./"RAYLIB_FOLDER"include",
        "-o", temp_sprintf("%s%s", BUILD_FOLDER, name),
        temp_sprintf("%s%s.c", SRC_FOLDER, name),
        "-L./"RAYLIB_FOLDER"lib", "-l:libraylib.a", "-lm"
    );

    if (!cmd_run_sync_and_reset(cmd)) return false;
    return true;
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    if (!mkdir_if_not_exists(BUILD_FOLDER)) return 1;

    Cmd cmd = {0};

    if (!build_game(&cmd, "breakout")) return 1;
    if (!build_game(&cmd, "flappy_bird")) return 1;
    if (!build_game(&cmd, "snake")) return 1;

    return 0;
}
