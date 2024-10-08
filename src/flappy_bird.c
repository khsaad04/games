#include <raylib.h>
#include <stdlib.h>

static const int scale = 1;
static const int screen_width = (800 * scale);
static const int screen_height = (600 * scale);
static const int fps = 60;
static const int bird_radius = (20 * scale);
static const int gravity = (30 * scale);

static const int pillar_width = 5 * bird_radius;
static const int pillar_gap = 10 * bird_radius;
static const int pillar_padding = (screen_width - 2 * pillar_width) / 2;

typedef enum State { STANDBY, RUNNING, PAUSED, OVER } State;

typedef struct {
    Vector2 pos, radius;
    Color color;
} Bird;

typedef struct {
    Vector2 pos;
    bool passed;
} Pillar;

static State game;
static int score;
static Bird bird;
static float bird_velocity;
static Pillar pillars[3];
static float pillar_velocity;
static float pillar_accel;

float random_piller_y(void)
{
    return (float)rand() / (float)RAND_MAX * (screen_height * 2.0 / 3.0 - 50) +
           50;
}

void init_game(void)
{
    game = STANDBY;
    score = 0;

    bird_velocity = 0;
    bird.pos = (Vector2){screen_width / 3.0, screen_height / 3.0};
    bird.radius = (Vector2){bird_radius, bird_radius};
    bird.color = RAYWHITE;

    pillar_accel = 1;
    pillar_velocity = 200 * scale;
    for (int i = 0; i < 3; ++i) {
        pillars[i].pos =
            (Vector2){screen_width + i * pillar_padding, random_piller_y()};
        pillars[i].passed = false;
    }
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
        bird.radius.y -= bird_velocity * .0075;
        bird.radius.x += bird_velocity * .0075;
        bird_velocity += gravity * GetFrameTime();

        pillar_velocity += pillar_accel * GetFrameTime();

        if (IsKeyPressed(KEY_SPACE)) {
            bird_velocity = -10;
        }

        if (IsKeyPressed(KEY_P)) {
            game = PAUSED;
        }

        if (bird.pos.y < 0 - bird_radius ||
            bird.pos.y > screen_height + bird_radius) {
            game = OVER;
            bird.color = RED;
        }

        for (int i = 0; i < 3; ++i) {
            pillars[i].pos.x -= pillar_velocity * GetFrameTime();

            if (pillars[i].pos.x < -pillar_width) {
                pillars[i].pos.x = screen_width;
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
                .width = pillar_width,
                .height = pillars[i].pos.y,
            };

            Rectangle pillar_bottom_rect = {
                .x = pillars[i].pos.x,
                .y = pillars[i].pos.y + pillar_gap,
                .width = pillar_width,
                .height = screen_height,
            };

            if (CheckCollisionCircleRec(bird.pos, bird_radius,
                                        pillar_top_rect) ||
                CheckCollisionCircleRec(bird.pos, bird_radius,
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
                 screen_width / 2.0 -
                     MeasureText("Press <Space> to start", 20) / 2.0,
                 screen_height / 2.0 - 20 / 2.0, 20, GRAY);
    }

    // Score
    if (score > 0) {
        DrawText(TextFormat("%d", score),
                 screen_width / 2.0 -
                     MeasureText(TextFormat("%d", score), 100) / 2.0,
                 screen_height / 2.0 - 100 / 2.0, 100, GRAY);
    }

    // Bird
    DrawEllipse(bird.pos.x, bird.pos.y, bird.radius.x, bird.radius.y,
                bird.color);

    // Pillars
    for (int i = 0; i < 3; ++i) {
        DrawRectangle(pillars[i].pos.x, 0, pillar_width, pillars[i].pos.y,
                      RAYWHITE);
        DrawRectangle(pillars[i].pos.x, pillars[i].pos.y + pillar_gap,
                      pillar_width, screen_height, RAYWHITE);
    }

    EndDrawing();
}

int main(void)
{
    InitWindow(screen_width, screen_height, "Flappy Bird");
    SetTargetFPS(fps);

    init_game();

    while (!WindowShouldClose()) {
        update_game();
        draw_game();
    }

    CloseWindow();
    return 0;
}
