#include <raylib.h>
#include <stdlib.h>

#define SCALE 1.0
#define SCREEN_WIDTH (800 * SCALE)
#define SCREEN_HEIGHT (600 * SCALE)
#define FPS 60
#define BIRD_RADIUS (20 * SCALE)
#define GRAVITY (30 * SCALE)
#define PILLAR_WIDTH (5 * BIRD_RADIUS)
#define PILLAR_GAP (10 * BIRD_RADIUS)
#define PILLAR_PADDING ((SCREEN_WIDTH - 2 * PILLAR_WIDTH) / 2)
#define PILLAR_VELOCITY (200 * SCALE)

typedef enum { STANDBY, RUNNING, PAUSED, OVER } State;

typedef struct {
    Vector2 pos, radius;
    Color color;
} Bird;

typedef struct {
    Vector2 pos;
    bool passed;
} Pillar;

static State game = STANDBY;
static int score = 0;
static Bird bird = {0};
static float bird_velocity = 0;
static Pillar pillars[3] = {0};
static float pillar_velocity = PILLAR_VELOCITY;
static float pillar_accel = 1;

float random_piller_y(void);
void init_game(void);
void update_game(void);
void draw_game(void);

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

void init_game(void)
{
    game = STANDBY;
    score = 0;

    bird_velocity = 0;
    bird.pos = (Vector2){SCREEN_WIDTH / 3.0, SCREEN_HEIGHT / 3.0};
    bird.radius = (Vector2){BIRD_RADIUS, BIRD_RADIUS};
    bird.color = RAYWHITE;

    pillar_accel = 1;
    pillar_velocity = PILLAR_VELOCITY;
    for (int i = 0; i < 3; ++i) {
        pillars[i].pos =
            (Vector2){SCREEN_WIDTH + i * PILLAR_PADDING, random_piller_y()};
        pillars[i].passed = false;
    }
}

float random_piller_y(void)
{
    return (float)rand() / (float)RAND_MAX * (SCREEN_HEIGHT * 2.0 / 3.0 - 50) +
           50;
}

void update_game(void)
{
    switch (game) {
    case STANDBY:
        if (IsKeyPressed(KEY_SPACE)) {
            bird_velocity = -10;
            game = RUNNING;
        }
        break;
    case RUNNING:
        bird.pos.y += bird_velocity;
        bird.radius.y -= bird_velocity * .01;
        bird.radius.x += bird_velocity * .01;
        bird_velocity += GRAVITY * GetFrameTime();

        pillar_velocity += pillar_accel * GetFrameTime();

        if (IsKeyPressed(KEY_SPACE)) {
            bird_velocity = -10;
        }

        if (IsKeyPressed(KEY_P)) {
            game = PAUSED;
        }

        if (bird.pos.y < 0 - BIRD_RADIUS ||
            bird.pos.y > SCREEN_HEIGHT + BIRD_RADIUS) {
            game = OVER;
            bird.color = RED;
        }

        for (int i = 0; i < 3; ++i) {
            pillars[i].pos.x -= pillar_velocity * GetFrameTime();

            if (pillars[i].pos.x < -PILLAR_WIDTH) {
                pillars[i].pos.x = SCREEN_WIDTH;
                pillars[i].pos.y = random_piller_y();
                pillars[i].passed = false;
            }

            if (pillars[i].pos.x <= bird.pos.x) {
                if (!pillars[i].passed) {
                    score += 1;
                }
                pillars[i].passed = true;
            }

            Rectangle pillar_top_rect = {
                .x = pillars[i].pos.x,
                .y = 0,
                .width = PILLAR_WIDTH,
                .height = pillars[i].pos.y,
            };

            Rectangle pillar_bottom_rect = {
                .x = pillars[i].pos.x,
                .y = pillars[i].pos.y + PILLAR_GAP,
                .width = PILLAR_WIDTH,
                .height = SCREEN_HEIGHT,
            };

            if (CheckCollisionCircleRec(bird.pos, BIRD_RADIUS,
                                        pillar_top_rect) ||
                CheckCollisionCircleRec(bird.pos, BIRD_RADIUS,
                                        pillar_bottom_rect)) {
                game = OVER;
                bird.color = RED;
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
    ClearBackground(BLACK);

    // Game intro
    if (game == STANDBY) {
        DrawText(TextFormat("Press <Space> to start"),
                 SCREEN_WIDTH / 2.0 -
                     MeasureText("Press <Space> to start", 20) / 2.0,
                 SCREEN_HEIGHT / 2.0 - 20 / 2.0, 20, GRAY);
    }

    // Score
    if (score > 0) {
        DrawText(TextFormat("%d", score),
                 SCREEN_WIDTH / 2.0 -
                     MeasureText(TextFormat("%d", score), 100) / 2.0,
                 SCREEN_HEIGHT / 2.0 - 100 / 2.0, 100, GRAY);
    }

    // Bird
    DrawEllipse(bird.pos.x, bird.pos.y, bird.radius.x, bird.radius.y,
                bird.color);

    // Pillars
    for (int i = 0; i < 3; ++i) {
        DrawRectangle(pillars[i].pos.x, 0, PILLAR_WIDTH, pillars[i].pos.y,
                      RAYWHITE);
        DrawRectangle(pillars[i].pos.x, pillars[i].pos.y + PILLAR_GAP,
                      PILLAR_WIDTH, SCREEN_HEIGHT, RAYWHITE);
    }

    EndDrawing();
}
