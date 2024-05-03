#include <raylib.h>
#include <stdbool.h>

static const int screen_width = 800;
static const int screen_height = 600;
static const int fps = 60;
static const int cell_size = 20;
static const int rows = screen_height / cell_size;
static const int cols = screen_width / cell_size;

enum State { STANDBY, RUNNING, PAUSED, OVER };

static enum State game;
static int score;

void init_game(void)
{
    game = STANDBY;
    score = 0;
}

void update_game(void)
{
    if (game == RUNNING) {
    }
    else if (game == PAUSED &&
             (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP) ||
              IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))) {
        game = RUNNING;
    }

    else if (game == OVER && IsKeyPressed(KEY_SPACE)) {
        init_game();
        game = STANDBY;
    }

    else if (game != OVER && IsKeyPressed(KEY_SPACE)) {
        game = RUNNING;
    }
}

void draw_game(void)
{
    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();
}

int main(void)
{
    InitWindow(screen_width, screen_height, "Flappy Bird");
    SetTargetFPS(fps);

    init_game();

    while (!WindowShouldClose()) {
        update_game();
        draw_game();
    }

    CloseWindow();
    return 0;
}
