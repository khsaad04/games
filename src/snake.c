#include <assert.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

// #define SNAKE_COLOR BLUE
// #define APPLE_COLOR RED
// #define BG1 \
//     (Color) { 20, 20, 20, 255 }
// #define BG2 \
//     (Color) { 10, 10, 10, 255 }

#define SNAKE_COLOR RAYWHITE
#define APPLE_COLOR RAYWHITE
#define BG1 BLACK
#define BG2 BLACK

static const int screen_width = 800;
static const int screen_height = 600;
static const int fps = 12;
static const int cell_size =
    23; // Must be such value that rows and cols are even numbered
static const Vector2 offset =
    (Vector2){screen_width % cell_size, screen_height % cell_size};
static const int rows = (screen_height - offset.y) / cell_size;
static const int cols = (screen_width - offset.x) / cell_size;
static const int snake_cap = rows * cols;

typedef enum State { STANDBY, RUNNING, PAUSED, OVER } State;

typedef struct {
    Vector2 pos, speed;
} SnakeCell;

typedef struct {
    Vector2 pos;
    bool active;
} Apple;

typedef struct {
    SnakeCell cells[snake_cap];
    size_t len;
} Snake;

static State game;
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

Vector2 get_random_apple_pos(void)
{
    return (Vector2){
        GetRandomValue(0, (screen_width / cell_size) - 1) * cell_size +
            offset.x / 2,
        GetRandomValue(0, (screen_height / cell_size) - 1) * cell_size +
            offset.y / 2};
}

void update_game(void)
{
    switch (game) {
    case STANDBY:
        if (IsKeyPressed(KEY_SPACE)) {
            game = RUNNING;
        }
        break;
    case RUNNING:
        for (size_t i = 0; i < snake.len; ++i) {
            snake_pos[i] = snake.cells[i].pos;
        }

        // snake step
        snake.cells[0].pos.x += snake.cells[0].speed.x;
        snake.cells[0].pos.y += snake.cells[0].speed.y;
        for (size_t i = 1; i < snake_cap; ++i) {
            snake.cells[i].pos = snake_pos[i - 1];
        }

        // snake-wall collision
        if (snake.cells[0].pos.x < offset.x / 2 ||
            snake.cells[0].pos.x > screen_width - offset.x / 2 - cell_size ||
            snake.cells[0].pos.y < offset.y / 2 ||
            snake.cells[0].pos.y > screen_height - offset.y / 2 - cell_size) {
            game = OVER;
        }

        // controls
        if (IsKeyPressed(KEY_P)) {
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

        // spawn random apple
        if (!apple.active) {
            apple.active = true;
            apple.pos = get_random_apple_pos();
            for (size_t i = 0; i < snake.len; i++) {
                while ((apple.pos.x == snake.cells[i].pos.x) &&
                       (apple.pos.y == snake.cells[i].pos.y)) {
                    apple.pos = get_random_apple_pos();
                    i = 0;
                }
            }
        }

        // snake-apple collision
        if (CheckCollisionRecs((Rectangle){.x = snake.cells[0].pos.x,
                                           .y = snake.cells[0].pos.y,
                                           .width = cell_size,
                                           .height = cell_size},
                               (Rectangle){.x = apple.pos.x,
                                           .y = apple.pos.y,
                                           .width = cell_size,
                                           .height = cell_size})) {
            snake.cells[snake.len].pos = snake_pos[snake.len - 1];
            snake.len += 1;
            score += 1;
            apple.active = false;
        }

        // snake-snake collision
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
        if (IsKeyPressed(KEY_SPACE)) {
            init_game();
            game = STANDBY;
        }
        break;
    }
}

void draw_game(void)
{
    BeginDrawing();
    // draw background
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

    // draw snake
    for (size_t i = 0; i < snake.len; ++i) {
        DrawRectangleV(snake.cells[i].pos, (Vector2){cell_size, cell_size},
                       SNAKE_COLOR);
    }

    // draw apple
    if (apple.active) {
        DrawCircleV((Vector2){apple.pos.x + cell_size / 2.0,
                              apple.pos.y + cell_size / 2.0},
                    cell_size * .4, APPLE_COLOR);
    }

    // Game intro
    if (game == STANDBY) {
        DrawText(TextFormat("Press <Space> to start"),
                 screen_width / 2 -
                     MeasureText("Press <Space> to start", 20) / 2,
                 screen_height / 3 * 2 - 20 / 2, 20, GRAY);
    }

    // draw score text
    DrawText(TextFormat("Score: %d", score), 20, screen_height - 25, 20,
             RAYWHITE);

    // game over texts
    if (game == OVER) {
        DrawText(TextFormat("GAME OVER"),
                 screen_width / 2 - MeasureText("GAME OVER", 30) / 2,
                 screen_height * .35, 30, RAYWHITE);
        DrawText(TextFormat("Score: %d", score),
                 screen_width / 2 -
                     MeasureText(TextFormat("Score: %d", score), 30) / 2,
                 screen_height * .45, 30, RAYWHITE);
        DrawText(TextFormat("Press <space> to restart"),
                 screen_width / 2 -
                     MeasureText("Press <space> to restart", 30) / 2,
                 screen_height * .55, 30, RAYWHITE);
    }
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
