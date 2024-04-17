#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BIRD_RADIUS 20
#define FPS 60

enum State { STANDBY, RUNNING, PAUSED, OVER };

typedef struct {
    Vector2 pos;
    Color color;
} Bird;

static enum State game;
static int score;
static Bird bird = {0};
static float velocity;
static float accel;

void init_game(void)
{
    game = STANDBY;
    score = 0;
    velocity = 0;
    accel = 1;
    bird.pos = (Vector2){SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0};
    bird.color = RAYWHITE;
}

void update_game(void)
{
    bird.pos.y += velocity;
    velocity += accel;
    if (IsKeyPressed(KEY_K)) {
        velocity = -20;
    }
    if (bird.pos.y < 0 - BIRD_RADIUS) {
        bird.pos.y = 0 - BIRD_RADIUS;
    }
    if (bird.pos.y > SCREEN_HEIGHT + BIRD_RADIUS) {
        bird.pos.y = SCREEN_HEIGHT + BIRD_RADIUS;
    }
}

void draw_game(void)
{
    BeginDrawing();
    ClearBackground(BLACK);
    DrawCircleV(bird.pos, BIRD_RADIUS, bird.color);
    EndDrawing();
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Flappy Bird");
    SetTargetFPS(FPS);

    init_game();

    while (!WindowShouldClose()) {
        update_game();
        draw_game();
    }

    CloseWindow();
    return 0;
}
