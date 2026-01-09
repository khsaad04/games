#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define BUILD_PATH  "build/"
#define SRC_PATH    "src/"
#define RAYLIB_PATH "raylib-5.5_linux_amd64/"

bool build_game(Cmd *cmd, const char *name)
{
    cmd_append(cmd, "cc");
    cmd_append(cmd, "-Wall", "-Wextra", "-O2");

    cmd_append(cmd, "-I./"RAYLIB_PATH"include",  "-o",
               temp_sprintf("%s%s", BUILD_PATH, name),
               temp_sprintf("%s%s.c", SRC_PATH, name),
               "-L./"RAYLIB_PATH"lib",
               "-l:libraylib.a", "-lm");

    if (!cmd_run(cmd)) return false;
    return true;
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    if (!mkdir_if_not_exists(BUILD_PATH)) return 1;

    Cmd cmd = {0};

    bool run = false;
    const char *name;

    const char *arg = shift(argv, argc);
    while (argc > 0) {
        arg = shift(argv, argc);

        if (strcmp(arg, "-run") == 0) {
            if (argc <= 0) {
                nob_log(ERROR, "expected arg: name of the binary to run");
                return 1;
            }

            arg = shift(argv, argc);
            if (strcmp(arg, "breakout")    == 0 ||
                strcmp(arg, "flappy_bird") == 0 ||
                strcmp(arg, "snake")       == 0) {
                run = true;
                name = arg;
            } else {
                nob_log(ERROR, "invalid name: %s", name);
                return 1;
            }
        } else {
            nob_log(ERROR, "invalid argument: %s", arg);
            return 1;
        }
    }

    if (!build_game(&cmd, "breakout"))    return 1;
    if (!build_game(&cmd, "flappy_bird")) return 1;
    if (!build_game(&cmd, "snake"))       return 1;

    if (run) {
        cmd_append(&cmd, temp_sprintf("./build/%s", name));
        if (!cmd_run(&cmd)) return 1;
    }

    return 0;
}
