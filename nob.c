#define NOB_IMPLEMENTATION
#include "nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"
#define RAYLIB_INCLUDE_FOLDER "raylib-5.5_linux_amd64/include/"
#define RAYLIB_LIB_FOLDER "raylib-5.5_linux_amd64/lib/"

bool build_game(Nob_Cmd *cmd, char *name)
{
    nob_cmd_append( cmd, "cc",
        "-Wall", "-Wextra", "-ggdb", "-O3",
        "-I./"RAYLIB_INCLUDE_FOLDER,
        "-o", nob_temp_sprintf("%s%s", BUILD_FOLDER, name),
        nob_temp_sprintf("%s%s.c", SRC_FOLDER, name),
        "-L./"RAYLIB_LIB_FOLDER, "-l:libraylib.a", "-lm");

    if (!nob_cmd_run_sync_and_reset(cmd)) return false;
    return true;
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;

    Nob_Cmd cmd = {0};
    build_game(&cmd, "breakout");
    build_game(&cmd, "flappy_bird");
    build_game(&cmd, "snake");

    return 0;
}
