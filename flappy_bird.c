#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BIRD_RADIUS 10
#define FPS 60

enum State { STANDBY, RUNNING, PAUSED, OVER };

typedef struct {
    Vector2 pos;
    Color color;
} Bird;

static enum State game;
static int score;
static Bird bird = {0};

void init_game(void)
{
    game = STANDBY;
    score = 0;
    bird.pos = (Vector2){SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0};
    bird.color = RAYWHITE;
}

void update_game(void) { bird.pos.y += 10; }

void draw_game(void)
{
    BeginDrawing();
    ClearBackground(BLACK);
    DrawCircleV(bird.pos, BIRD_RADIUS, bird.color);
    EndDrawing();
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout");
    SetTargetFPS(FPS);

    init_game();

    while (!WindowShouldClose()) {
        update_game();
        draw_game();
    }

    CloseWindow();
    return 0;
}
