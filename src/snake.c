#include <assert.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>

#define SNAKE_COLOR BLUE
#define APPLE_COLOR RED
#define BG1                                                                    \
    (Color) { 20, 20, 20, 255 }
#define BG2                                                                    \
    (Color) { 10, 10, 10, 255 }

static const int screen_width = 800;
static const int screen_height = 600;
static const int fps = 12;
static const int cell_size =
    33; // Must be such value that rows and cols are even numbered
static const Vector2 offset =
    (Vector2){screen_width % cell_size, screen_height % cell_size};
static const int rows = (screen_height - offset.y) / cell_size;
static const int cols = (screen_width - offset.x) / cell_size;
static const int snake_cap = rows * cols;

enum State { STANDBY, RUNNING, PAUSED, OVER };

typedef struct {
    Vector2 pos;
    Vector2 speed;
} SnakeCell;

typedef struct {
    Vector2 pos;
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
    snake.len = 1;
    assert(rows % 2 == 0);
    assert(cols % 2 == 0);
    for (size_t i = 0; i < snake_cap; ++i) {
        snake.cells[i].pos =
            (Vector2){offset.x / 2 + (cols / 2.0 - 3) * cell_size,
                      offset.y / 2 + (rows / 2.0) * cell_size};
        snake.cells[i].speed = (Vector2){cell_size, 0};
    }

    apple.active = false;
}

void controls_update(void)
{
    if (IsKeyPressed(KEY_P) && game != OVER) {
        game = PAUSED;
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
}

void update_game(void)
{
    switch (game) {
    case STANDBY:
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP) ||
            IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT)) {
            controls_update();
            game = RUNNING;
        }
        break;
    case RUNNING:
        for (size_t i = 0; i < snake.len; ++i) {
            snake_pos[i] = snake.cells[i].pos;
        }

        snake.cells[0].pos.x += snake.cells[0].speed.x;
        snake.cells[0].pos.y += snake.cells[0].speed.y;
        for (size_t i = 1; i < snake_cap; ++i) {
            snake.cells[i].pos = snake_pos[i - 1];
        }

        if (snake.cells[0].pos.x < offset.x / 2 ||
            snake.cells[0].pos.x + cell_size > screen_width - offset.x / 2 ||
            snake.cells[0].pos.y < offset.y / 2 ||
            snake.cells[0].pos.y + cell_size > screen_height - offset.x / 2)
            game = OVER;

        controls_update();

        if (!apple.active) {
            apple.active = true;
            apple.pos = (Vector2){
                GetRandomValue(0, (screen_width / cell_size) - 1) * cell_size +
                    offset.x / 2,
                GetRandomValue(0, (screen_height / cell_size) - 1) * cell_size +
                    offset.y / 2};

            for (size_t i = 0; i < snake.len; i++) {
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

        if ((snake.cells[0].pos.x < (apple.pos.x + cell_size) &&
             (snake.cells[0].pos.x + cell_size) > apple.pos.x) &&
            (snake.cells[0].pos.y < (apple.pos.y + cell_size) &&
             (snake.cells[0].pos.y + cell_size) > apple.pos.y)) {
            snake.cells[snake.len].pos = snake_pos[snake.len - 1];
            snake.len += 1;
            score += 1;
            apple.active = false;
        }

        for (size_t i = 1; i < snake.len; ++i) {
            if (snake.cells[0].pos.x == snake.cells[i].pos.x &&
                snake.cells[0].pos.y == snake.cells[i].pos.y) {
                game = OVER;
            }
        }
        break;
    case PAUSED:
        if (IsKeyPressed(KEY_P)) {
            game = RUNNING;
        }
        break;
    case OVER:
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP) ||
            IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT)) {
            init_game();
            game = STANDBY;
        }
        break;
    }
}

void draw_game(void)
{
    BeginDrawing();
    ClearBackground(BG1);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            if ((i + j) % 2 == 0) {
                DrawRectangleV((Vector2){offset.x / 2 + j * cell_size,
                                         offset.y / 2 + i * cell_size},
                               (Vector2){cell_size, cell_size}, BG2);
            }
        }
    }
    DrawLineV((Vector2){offset.x / 2, offset.y / 2},
              (Vector2){screen_width - offset.x / 2, offset.y / 2}, BG2);

    DrawLineV(
        (Vector2){offset.x / 2, screen_height - offset.y / 2},
        (Vector2){screen_width - offset.x / 2, screen_height - offset.y / 2},
        BG2);

    DrawLineV((Vector2){offset.x / 2, offset.y / 2},
              (Vector2){offset.x / 2, screen_height - offset.y / 2}, BG2);

    DrawLineV(
        (Vector2){screen_width - offset.x / 2, offset.y / 2},
        (Vector2){screen_width - offset.x / 2, screen_height - offset.y / 2},
        BG2);

    for (size_t i = 0; i < snake.len; ++i) {
        DrawRectangleV(snake.cells[i].pos, (Vector2){cell_size, cell_size},
                       SNAKE_COLOR);
    }
    if (apple.active)
        DrawRectangleV(apple.pos, (Vector2){cell_size, cell_size}, APPLE_COLOR);

    DrawText(TextFormat("Score: %d", score), 20, screen_height - 25, 20,
             RAYWHITE);
    EndDrawing();
}

int main(void)
{
    InitWindow(screen_width, screen_height, "Snake");
    SetTargetFPS(fps);

    init_game();

    while (!WindowShouldClose()) {
        update_game();
        draw_game();
    }

    CloseWindow();
    return 0;
}
