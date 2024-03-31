#include <raylib.h>
#include <stdbool.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define CELL_SIZE 25

typedef struct {
    Vector2 pos;
    Vector2 size;
    Vector2 speed;
    Color color;
} Snake;

static const Color BG_COLOR_1 = {24, 24, 37, 255};
static const Color BG_COLOR_2 = {30, 30, 46, 255};
static const Vector2 offset = {WIN_WIDTH % CELL_SIZE, WIN_HEIGHT % CELL_SIZE};

static void draw_background();
static void move_snake();

int x = 0;
int y = WIN_HEIGHT / 2;
int dx = 1;
int dy = 1;
bool movingVert = false;

int main(void) 
{
    InitWindow(WIN_WIDTH, WIN_HEIGHT, "Snake");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        draw_background();
        move_snake();
        DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, BLUE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void draw_background() 
{
    ClearBackground(BG_COLOR_1);
    for (int i = 0; i < WIN_WIDTH / CELL_SIZE; ++i) {
        for (int j = 0; j < WIN_HEIGHT / CELL_SIZE; ++j) {
            if ((i + j) % 2 == 0)
                DrawRectangle(i * CELL_SIZE + offset.x / 2,
                        j * CELL_SIZE + offset.y / 2, CELL_SIZE, CELL_SIZE,
                        BG_COLOR_2);
        }
    }
    return;
}

void move_snake() 
{
    if (!movingVert) {
        x += CELL_SIZE * 10 * GetFrameTime() * dx;
    }
    if (movingVert) {
        y += CELL_SIZE * 10 * GetFrameTime() * dy;
    }

    if (x > WIN_WIDTH - offset.x / 2 - CELL_SIZE) {
        x = WIN_WIDTH - offset.x / 2 - CELL_SIZE;
    }
    if (x < offset.x / 2) {
        x = offset.x / 2;
    }
    if (y > WIN_HEIGHT + offset.y / 2 - CELL_SIZE) {
        y = WIN_HEIGHT + offset.y / 2 - CELL_SIZE;
    }
    if (y < offset.y / 2) {
        y = offset.y / 2;
    }

    if (IsKeyPressed(KEY_LEFT) && (movingVert)) {
        movingVert = false;
        dx = -1;
    }
    if (IsKeyPressed(KEY_RIGHT) && (movingVert)) {
        movingVert = false;
        dx = 1;
    }
    if (IsKeyPressed(KEY_UP) && (!movingVert)) {
        movingVert = true;
        dy = -1;
    }
    if (IsKeyPressed(KEY_DOWN) && (!movingVert)) {
        movingVert = true;
        dy = 1;
    }

    return;
}
