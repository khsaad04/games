#include <raylib.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define CELL_SIZE 20

static const Color BG_COLOR_1 = {24, 24, 37, 255};
static const Color BG_COLOR_2 = {30, 30, 46, 255};
static const Vector2 offset = {WIN_WIDTH % CELL_SIZE, WIN_HEIGHT % CELL_SIZE};

static void draw_background();

int main()
{
    InitWindow(WIN_WIDTH, WIN_HEIGHT, "Snake");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        draw_background();
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
}
