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

#define SCALE 1
#define SCREEN_WIDTH ((int)(800 * SCALE))
#define SCREEN_HEIGHT ((int)(600 * SCALE))
#define FPS 12
#define CELL_SIZE (int)(20 * SCALE)
#define ROWS ((int)(SCREEN_HEIGHT / CELL_SIZE))
#define COLS ((int)(SCREEN_WIDTH / CELL_SIZE))
#define SNAKE_CAP (ROWS * COLS)

static const Vector2 offset = (Vector2){SCREEN_WIDTH % (CELL_SIZE * COLS),
                                        SCREEN_HEIGHT % (CELL_SIZE * ROWS)};

typedef enum State { STANDBY, RUNNING, PAUSED, OVER } State;

typedef struct {
    Vector2 pos, size, speed;
} SnakeCell;

typedef struct {
    SnakeCell cells[SNAKE_CAP];
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
static Vector2 snake_pos[SNAKE_CAP];
static Apple apple;

void init_game(void)
{
    game = STANDBY;
    score = 0;
    snake.len = 1;
    for (int i = 0; i < SNAKE_CAP; ++i) {
        snake.cells[i].pos = (Vector2){
            offset.x / 2 + ((SCREEN_WIDTH / 20.0) / 2 - 3) * CELL_SIZE,
            offset.y / 2 + ((SCREEN_HEIGHT / 20.0) / 2) * CELL_SIZE};
        snake.cells[i].speed = (Vector2){CELL_SIZE, 0};
        snake.cells[i].size = (Vector2){CELL_SIZE, CELL_SIZE};
    }
    apple.radius = CELL_SIZE * .4;
    apple.active = false;
}

Vector2 get_random_apple_pos(void)
{
    return (Vector2){
        GetRandomValue(0, (SCREEN_WIDTH / CELL_SIZE) - 1) * CELL_SIZE +
            offset.x / 2,
        GetRandomValue(0, (SCREEN_HEIGHT / CELL_SIZE) - 1) * CELL_SIZE +
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
        for (int i = 1; i < SNAKE_CAP; ++i) {
            snake.cells[i].pos = snake_pos[i - 1];
        }

        // snake-wall collision
        if (snake.cells[0].pos.x < offset.x / 2 ||
            snake.cells[0].pos.x > SCREEN_WIDTH - offset.x / 2 - CELL_SIZE ||
            snake.cells[0].pos.y < offset.y / 2 ||
            snake.cells[0].pos.y > SCREEN_HEIGHT - offset.y / 2 - CELL_SIZE) {
            game = OVER;
        }

        // controls
        if (IsKeyPressed(KEY_P)) {
            game = PAUSED;
        }
        if (IsKeyPressed(KEY_RIGHT) && (snake.cells[0].speed.x == 0)) {
            snake.cells[0].speed = (Vector2){CELL_SIZE, 0};
        }
        if (IsKeyPressed(KEY_LEFT) && (snake.cells[0].speed.x == 0)) {
            snake.cells[0].speed = (Vector2){-CELL_SIZE, 0};
        }
        if (IsKeyPressed(KEY_UP) && (snake.cells[0].speed.y == 0)) {
            snake.cells[0].speed = (Vector2){0, -CELL_SIZE};
        }
        if (IsKeyPressed(KEY_DOWN) && (snake.cells[0].speed.y == 0)) {
            snake.cells[0].speed = (Vector2){0, CELL_SIZE};
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
                                           .width = CELL_SIZE,
                                           .height = CELL_SIZE})) {
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
    DrawRectangleLines(offset.x / 2, offset.y / 2, SCREEN_WIDTH - offset.x,
                       SCREEN_HEIGHT - offset.y, GRAY);

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
                 SCREEN_WIDTH / 2 -
                     MeasureText("Press <Space> to start", 20) / 2,
                 SCREEN_HEIGHT / 3 * 2 - 20 / 2, 20, GRAY);
    }

    // draw score text
    DrawText(TextFormat("Score: %d", score), 20, SCREEN_HEIGHT + 5, 20,
             RAYWHITE);

    // draw game over texts
    if (game == OVER) {
        DrawText(TextFormat("GAME OVER"),
                 SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 30) / 2,
                 SCREEN_HEIGHT * .35, 30, RAYWHITE);
        DrawText(TextFormat("Score: %d", score),
                 SCREEN_WIDTH / 2 -
                     MeasureText(TextFormat("Score: %d", score), 30) / 2,
                 SCREEN_HEIGHT * .45, 30, RAYWHITE);
        DrawText(TextFormat("Press <space> to restart"),
                 SCREEN_WIDTH / 2 -
                     MeasureText("Press <space> to restart", 30) / 2,
                 SCREEN_HEIGHT * .55, 30, RAYWHITE);
    }
    EndDrawing();
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake");
    SetTargetFPS(FPS);

    init_game();

    while (!WindowShouldClose()) {
        update_game();
        draw_game();
    }

    CloseWindow();
    return 0;
}
