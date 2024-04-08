#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define CELL_SIZE 30
#define FPS 60
#define snake_cap 256

static const Vector2 offset = {.x = SCREEN_WIDTH % CELL_SIZE,
                               .y = SCREEN_HEIGHT % CELL_SIZE};
static const int rows = (SCREEN_HEIGHT - offset.y) / CELL_SIZE;
static const int cols = (SCREEN_WIDTH - offset.x) / CELL_SIZE;

static int tail_counter;
static int frame_counter;

typedef struct Snake {
    Vector2 pos;
    Vector2 size;
    Vector2 speed;
    Color color;
} Snake;

static Snake snake[snake_cap] = {0};
static Vector2 snake_head[snake_cap] = {0};

void init_game(void)
{
    frame_counter = 0;
    tail_counter = 3;
    for (int i = 0; i < snake_cap; ++i) {
        snake[i].pos = (Vector2){offset.x / 2, offset.y / 2};
        snake[i].size = (Vector2){CELL_SIZE, CELL_SIZE};
        snake[i].speed = (Vector2){CELL_SIZE, 0};
        snake[i].color = BLUE;
        snake_head[i] = (Vector2){0.0f, 0.0f};
    }
}
void update_game(void)
{
    for (int i = 0; i < tail_counter; ++i) {
        snake_head[i] = snake[i].pos;
        if (frame_counter % 5 == 0) {
            if (i == 0) {
                snake[0].pos.x += snake[0].speed.x;
                snake[0].pos.y += snake[0].speed.y;
            }
            else
                snake[i].pos = snake_head[i - 1];
        }
    }
    frame_counter++;
}
void draw_game(void)
{
    BeginDrawing();
    ClearBackground(BLACK);
    for (int i = 0; i < tail_counter; i++)
        DrawRectangleV(snake[i].pos, snake[i].size, snake[i].color);
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
