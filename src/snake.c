#include <raylib.h>

/*#define CHECKERBOARD*/

#ifdef CHECKERBOARD

#define SNAKE_COLOR BLUE
#define APPLE_COLOR RED
#define BG1 (Color){20, 20, 20, 255}
#define BG2 (Color){10, 10, 10, 255}

#else

#define SNAKE_COLOR RAYWHITE
#define APPLE_COLOR RAYWHITE
#define BG1 BLACK
#define BG2 BLACK

#endif // CHECKERBOARD

#define SCALE 1.0
#define SCREEN_WIDTH (800 * SCALE)
#define SCREEN_HEIGHT (600 * SCALE)
#define FPS 12
#define CELL_SIZE (20 * SCALE)
#define ROWS 30
#define COLS 40
#define SNAKE_CAP (ROWS * COLS)

typedef enum State { STANDBY, RUNNING, PAUSED, OVER } State;

typedef struct {
    Vector2 pos, speed;
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

State game = STANDBY;
int score = 0;
Snake snake = {0};
Vector2 snake_pos[SNAKE_CAP] = {0};
Apple apple = {0};

void init_game(void);
void update_game(void);
Vector2 get_random_apple_pos(void);
void draw_game(void);

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

void init_game(void)
{
    game = STANDBY;
    score = 0;
    snake.len = 1;
    for (int i = 0; i < SNAKE_CAP; ++i) {
        snake.cells[i].pos =
            (Vector2){((SCREEN_WIDTH / 20.0) / 2 - 3) * CELL_SIZE,
                      ((SCREEN_HEIGHT / 20.0) / 2) * CELL_SIZE};
        snake.cells[i].speed = (Vector2){CELL_SIZE, 0};
    }
    apple.radius = CELL_SIZE * .4;
    apple.active = false;
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
        if (snake.cells[0].pos.x < 0 ||
            snake.cells[0].pos.x > SCREEN_WIDTH - CELL_SIZE ||
            snake.cells[0].pos.y < 0 ||
            snake.cells[0].pos.y > SCREEN_HEIGHT - CELL_SIZE) {
            game = OVER;
        }

        // controls
        if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_SPACE)) {
            game = PAUSED;
        }
        if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) &&
            (snake.cells[0].speed.y == 0)) {
            snake.cells[0].speed = (Vector2){0, -CELL_SIZE};
        }
        if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) &&
            (snake.cells[0].speed.x == 0)) {
            snake.cells[0].speed = (Vector2){-CELL_SIZE, 0};
        }
        if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) &&
            (snake.cells[0].speed.y == 0)) {
            snake.cells[0].speed = (Vector2){0, CELL_SIZE};
        }
        if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) &&
            (snake.cells[0].speed.x == 0)) {
            snake.cells[0].speed = (Vector2){CELL_SIZE, 0};
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
                                           .width = CELL_SIZE,
                                           .height = CELL_SIZE},
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
        if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_SPACE)) {
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

Vector2 get_random_apple_pos(void)
{
    return (Vector2){
        GetRandomValue(0, (SCREEN_WIDTH / CELL_SIZE) - 1) * CELL_SIZE,
        GetRandomValue(0, (SCREEN_HEIGHT / CELL_SIZE) - 1) * CELL_SIZE};
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
                DrawRectangleV((Vector2){j * CELL_SIZE, i * CELL_SIZE},
                               (Vector2){CELL_SIZE, CELL_SIZE}, BG2);
            }
        }
    }
#endif
    // draw snake
    for (int i = 0; i < snake.len; ++i) {
        DrawRectangleV(snake.cells[i].pos, (Vector2){CELL_SIZE, CELL_SIZE},
                       SNAKE_COLOR);
    }

    // draw apple
    if (apple.active) {
        DrawCircleV(
            (Vector2){apple.pos.x + apple.radius, apple.pos.y + apple.radius},
            apple.radius, APPLE_COLOR);
    }

    const char *text;

    // draw game intro
    text = "Press <Space> to start";
    if (game == STANDBY) {
        DrawText(text, SCREEN_WIDTH / 2 - MeasureText(text, 20) / 2.0,
                 SCREEN_HEIGHT / 3 * 2 - 20.0 / 2, 20, GRAY);
    }

    // draw score text
    text = TextFormat("Score: %d", score);
    DrawText(text, 20, SCREEN_HEIGHT - 25, 20, GRAY);

    // draw game over texts
    if (game == OVER) {
        text = "GAME OVER";
        DrawText(text, SCREEN_WIDTH / 2 - MeasureText(text, 30) / 2.0,
                 SCREEN_HEIGHT * .35, 30, RAYWHITE);
        text = TextFormat("Score: %d", score);
        DrawText(text, SCREEN_WIDTH / 2 - MeasureText(text, 30) / 2.0,
                 SCREEN_HEIGHT * .45, 30, RAYWHITE);
        text = "Press <space> to restart";
        DrawText(text, SCREEN_WIDTH / 2 - MeasureText(text, 30) / 2.0,
                 SCREEN_HEIGHT * .55, 30, RAYWHITE);
    }
    EndDrawing();
}
