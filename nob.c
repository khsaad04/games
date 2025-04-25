#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"
#define RAYLIB_FOLDER "raylib-5.5_linux_amd64/"

void build_game(Cmd *cmd, Procs *procs, char *name)
{
    cmd_append(
        cmd, "cc",
        "-Wall", "-Wextra", "-ggdb", "-O3",
        "-I./"RAYLIB_FOLDER"include",
        "-o", temp_sprintf("%s%s", BUILD_FOLDER, name),
        temp_sprintf("%s%s.c", SRC_FOLDER, name),
        "-L./"RAYLIB_FOLDER"lib", "-l:libraylib.a", "-lm"
    );

    da_append(procs, cmd_run_async_and_reset(cmd));
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    if (!mkdir_if_not_exists(BUILD_FOLDER)) return 1;

    Cmd cmd = {0};
    Procs procs = {0};

    build_game(&cmd, &procs, "breakout");
    build_game(&cmd, &procs, "flappy_bird");
    build_game(&cmd, &procs, "snake");

    if (!procs_wait_and_reset(&procs)) return 1;

    return 0;
}
