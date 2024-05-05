#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>

static const int screen_width = 800;
static const int screen_height = 600;
static const int fps = 24;
static const int cell_size = 31;
static const Vector2 offset =
    (Vector2){screen_width % cell_size, screen_height % cell_size};
static const int rows = (screen_height - offset.y) / cell_size;
static const int cols = (screen_width - offset.x) / cell_size;
static const int snake_cap = rows * cols;

enum State { STANDBY, RUNNING, PAUSED, OVER };

typedef struct {
    Vector2 pos;
    Vector2 size;
    Vector2 speed;
} SnakeCell;

typedef struct {
    Vector2 pos;
    Vector2 size;
    bool active;
} Apple;

typedef struct {
    SnakeCell cells[snake_cap];
    size_t len;
} Snake;

static enum State game;
static int score;
static Snake snake;
static Vector2 snake_pos[snake_cap];
static Apple apple;

void init_game(void)
{
    game = STANDBY;
    score = 0;
    snake.len = 3;
    for (size_t i = 0; i < snake_cap; ++i) {
        snake.cells[i].pos = (Vector2){offset.x / 2, offset.y / 2};
        snake.cells[i].size = (Vector2){cell_size, cell_size};
        snake.cells[i].speed = (Vector2){cell_size, 0};
    }

    apple.size = (Vector2){cell_size, cell_size};
    apple.active = false;
}

void update_game(void)
{
    if (game == RUNNING) {
        for (size_t i = 0; i < snake.len; ++i) {
            snake_pos[i] = snake.cells[i].pos;
        }

        for (size_t i = 0; i < snake_cap; ++i) {
            if (i == 0) {
                snake.cells[0].pos.x += snake.cells[0].speed.x;
                snake.cells[0].pos.y += snake.cells[0].speed.y;
            }
            else {
                snake.cells[i].pos = snake_pos[i - 1];
            }
        }

        for (size_t i = 0; i < snake.len; ++i) {
            if (snake.cells[i].pos.x < offset.x / 2)
                snake.cells[i].pos.x = offset.x / 2;
            if (snake.cells[i].pos.x > screen_width - offset.x / 2 - cell_size)
                snake.cells[i].pos.x = screen_width - offset.x / 2 - cell_size;
            if (snake.cells[i].pos.y < offset.y / 2)
                snake.cells[i].pos.y = offset.y / 2;
            if (snake.cells[i].pos.y > screen_height - offset.x / 2 - cell_size)
                snake.cells[i].pos.y = screen_height - offset.y / 2 - cell_size;
        }

        if (IsKeyPressed(KEY_RIGHT) && (snake.cells[0].speed.x == 0)) {
            snake.cells[0].speed = (Vector2){cell_size, 0};
        }
        if (IsKeyPressed(KEY_LEFT) && (snake.cells[0].speed.x == 0)) {
            snake.cells[0].speed = (Vector2){-cell_size, 0};
        }
        if (IsKeyPressed(KEY_UP) && (snake.cells[0].speed.y == 0)) {
            snake.cells[0].speed = (Vector2){0, -cell_size};
        }
        if (IsKeyPressed(KEY_DOWN) && (snake.cells[0].speed.y == 0)) {
            snake.cells[0].speed = (Vector2){0, cell_size};
        }

        if (!apple.active) {
            apple.active = true;
            apple.pos = (Vector2){
                GetRandomValue(0, (screen_width / cell_size) - 1) * cell_size +
                    offset.x / 2,
                GetRandomValue(0, (screen_height / cell_size) - 1) * cell_size +
                    offset.y / 2};

            for (int i = 0; i < snake.len; i++) {
                while ((apple.pos.x == snake.cells[i].pos.x) &&
                       (apple.pos.y == snake.cells[i].pos.y)) {
                    apple.pos = (Vector2){
                        GetRandomValue(0, (screen_width / cell_size) - 1) *
                                cell_size +
                            offset.x / 2,
                        GetRandomValue(0, (screen_height / cell_size) - 1) *
                                cell_size +
                            offset.y / 2};
                    i = 0;
                }
            }
        }

        if ((snake.cells[0].pos.x < (apple.pos.x + apple.size.x) &&
             (snake.cells[0].pos.x + snake.cells[0].size.x) > apple.pos.x) &&
            (snake.cells[0].pos.y < (apple.pos.y + apple.size.y) &&
             (snake.cells[0].pos.y + snake.cells[0].size.y) > apple.pos.y)) {
            snake.cells[snake.len].pos = snake_pos[snake.len - 1];
            snake.len += 1;
            apple.active = false;
        }
    }
    else if (game == PAUSED && IsKeyPressed(KEY_P)) {
        game = RUNNING;
    }

    else if (game == OVER &&
             (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP) ||
              IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))) {
        init_game();
        game = STANDBY;
    }

    else if (game != OVER &&
             (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP) ||
              IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))) {
        game = RUNNING;
    }
}

void draw_game(void)
{
    BeginDrawing();
    ClearBackground(BLACK);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            if ((i + j) % 2 == 0) {
                DrawRectangleV((Vector2){offset.x / 2 + j * cell_size,
                                         offset.y / 2 + i * cell_size},
                               (Vector2){cell_size, cell_size},
                               GetColor(0x252525));
            }
        }
    }

    for (size_t i = 0; i < snake.len; ++i) {
        DrawRectangleV(snake.cells[i].pos, snake.cells[i].size, BLUE);
    }
    if (apple.active)
        DrawRectangleV(apple.pos, apple.size, RED);
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
