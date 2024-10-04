#include <raylib.h>

/*#define CHECKERBOARD*/

#ifdef CHECKERBOARD

#define SNAKE_COLOR BLUE
#define APPLE_COLOR RED
#define BG1                                                                    \
    (Color) { 20, 20, 20, 255 }
#define BG2                                                                    \
    (Color) { 10, 10, 10, 255 }

#else

#define SNAKE_COLOR RAYWHITE
#define APPLE_COLOR RAYWHITE
#define BG1 BLACK
#define BG2 BLACK

#endif

static const int scale = 1;
static const int screen_width = ((int)(800 * scale));
static const int screen_height = ((int)(600 * scale));
static const int fps = 12;
static const int cell_size = (int)(20 * scale);
static const int rows = ((int)(screen_height / cell_size));
static const int cols = ((int)(screen_width / cell_size));
/*static const int snake_cap = (rows * cols);*/
static const Vector2 offset = (Vector2){screen_width % (cell_size * cols),
                                        screen_height % (cell_size * rows)};

enum {
    snake_cap = (rows * cols)
}; // NOTE: Did this to fix folding-constant-array warning

typedef enum State { STANDBY, RUNNING, PAUSED, OVER } State;

typedef struct {
    Vector2 pos, size, speed;
} SnakeCell;

typedef struct {
    SnakeCell cells[snake_cap];
    int len;
} Snake;

typedef struct {
    Vector2 pos;
    float radius;
    bool active;
} Apple;

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
    for (int i = 0; i < snake_cap; ++i) {
        snake.cells[i].pos = (Vector2){
            offset.x / 2 + ((screen_width / 20.0) / 2 - 3) * cell_size,
            offset.y / 2 + ((screen_height / 20.0) / 2) * cell_size};
        snake.cells[i].speed = (Vector2){cell_size, 0};
        snake.cells[i].size = (Vector2){cell_size, cell_size};
    }
    apple.radius = cell_size * .4;
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
        for (int i = 0; i < snake.len; ++i) {
            snake_pos[i] = snake.cells[i].pos;
        }

        // snake step
        snake.cells[0].pos.x += snake.cells[0].speed.x;
        snake.cells[0].pos.y += snake.cells[0].speed.y;
        for (int i = 1; i < snake_cap; ++i) {
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
            for (int i = 0; i < snake.len; i++) {
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
                                           .width = snake.cells[0].size.x,
                                           .height = snake.cells[0].size.y},
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
        for (int i = 1; i < snake.len; ++i) {
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
#ifdef CHECKERBOARD
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if ((i + j) % 2 == 0) {
                DrawRectangleV((Vector2){offset.x / 2 + j * CELL_SIZE,
                                         offset.y / 2 + i * CELL_SIZE},
                               (Vector2){CELL_SIZE, CELL_SIZE}, BG2);
            }
        }
    }
#endif
    DrawRectangleLines(offset.x / 2, offset.y / 2, screen_width - offset.x,
                       screen_height - offset.y, GRAY);

    // draw snake
    for (int i = 0; i < snake.len; ++i) {
        DrawRectangleV(snake.cells[i].pos, snake.cells[i].size, SNAKE_COLOR);
    }

    // draw apple
    if (apple.active) {
        DrawCircleV(
            (Vector2){apple.pos.x + apple.radius, apple.pos.y + apple.radius},
            apple.radius, APPLE_COLOR);
    }

    // draw game intro
    if (game == STANDBY) {
        DrawText(TextFormat("Press <Space> to start"),
                 screen_width / 2 -
                     MeasureText("Press <Space> to start", 20) / 2,
                 screen_height / 3 * 2 - 20 / 2, 20, GRAY);
    }

    // draw score text
    DrawText(TextFormat("Score: %d", score), 20, screen_height + 5, 20,
             RAYWHITE);

    // draw game over texts
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
