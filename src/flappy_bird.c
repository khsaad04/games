#include <raylib.h>
#include <time.h>

#define SCREEN_WIDTH GetScreenWidth()
#define SCREEN_HEIGHT GetScreenHeight()
#define FPS 60
#define BIRD_RADIUS (20.0 * SCREEN_WIDTH / 800.0)
#define GRAVITY (30.0 * SCREEN_HEIGHT / 600.0)
#define PILLAR_WIDTH (5.0 * BIRD_RADIUS)
#define PILLAR_GAP (10.0 * BIRD_RADIUS)
#define PILLAR_PADDING ((SCREEN_WIDTH - 2.0 * PILLAR_WIDTH) / 2.0)
#define PILLAR_VELOCITY (200 * SCREEN_WIDTH / 800.0)
#define FONT_SIZE (20.0 * SCREEN_HEIGHT / 600.0)

typedef enum { STANDBY, RUNNING, PAUSED, OVER } State;

typedef struct {
    Vector2   pos, radius;
    Color     color;
} Bird;

typedef struct {
    Rectangle top, bottom;
    bool      passed;
} Pillar;

static State  state;
static int    score;
static Bird   bird;
static float  bird_velocity;
static Pillar pillars[3];
static float  pillar_velocity;
static float  pillar_accel;

float random_piller_y(void)
{
    return GetRandomValue(0, SCREEN_HEIGHT * 2.0 / 3.0);
}

void init_game(void)
{
    state = STANDBY;
    score = 0;

    bird_velocity = 0;
    bird.pos      = (Vector2){SCREEN_WIDTH / 3.0, SCREEN_HEIGHT / 3.0};
    bird.radius   = (Vector2){BIRD_RADIUS, BIRD_RADIUS};
    bird.color    = RAYWHITE;

    pillar_accel    = 1;
    pillar_velocity = PILLAR_VELOCITY;

    for (int i = 0; i < 3; ++i) {

        float y_cord        = random_piller_y();
        pillars[i].top.x    = pillars[i].bottom.x = SCREEN_WIDTH + i * PILLAR_PADDING;
        pillars[i].top.y    = 0;
        pillars[i].bottom.y = y_cord + PILLAR_GAP;

        pillars[i].top.width     = pillars[i].bottom.width = PILLAR_WIDTH;
        pillars[i].top.height    = y_cord;
        pillars[i].bottom.height = SCREEN_HEIGHT;

        pillars[i].passed = false;
    }
}

void update_game(void)
{
    switch (state) {
    case STANDBY:
        if (IsKeyPressed(KEY_SPACE)) {
            bird_velocity = -10.0 * SCREEN_HEIGHT / 600.0;
            state         = RUNNING;
        }
        break;

    case RUNNING:
        bird.pos.y    += bird_velocity;
        bird.radius.y -= bird_velocity * .01;
        bird.radius.x += bird_velocity * .01;
        bird_velocity += GRAVITY * GetFrameTime();

        pillar_velocity += pillar_accel * GetFrameTime();

        if (IsKeyPressed(KEY_SPACE)) {
            bird_velocity = -10.0 * SCREEN_HEIGHT / 600.0;
        }

        if (IsKeyPressed(KEY_P)) {
            state = PAUSED;
        }

        if (bird.pos.y < 0 - BIRD_RADIUS ||
            bird.pos.y > SCREEN_HEIGHT + BIRD_RADIUS) {
            state      = OVER;
            bird.color = RED;
        }

        for (int i = 0; i < 3; ++i) {
            pillars[i].top.x = pillars[i].bottom.x -= pillar_velocity * GetFrameTime();

            if (pillars[i].top.x < -PILLAR_WIDTH) {
                float y_cord          = random_piller_y();
                pillars[i].top.x      = pillars[i].bottom.x = SCREEN_WIDTH;
                pillars[i].top.y      = 0;
                pillars[i].bottom.y   = y_cord + PILLAR_GAP;
                pillars[i].top.height = y_cord;
                pillars[i].passed     = false;
            }

            if (pillars[i].top.x <= bird.pos.x) {
                if (!pillars[i].passed) {
                    score += 1;
                }
                pillars[i].passed = true;
            }

            if (CheckCollisionCircleRec(bird.pos, BIRD_RADIUS, pillars[i].top) ||
                CheckCollisionCircleRec(bird.pos, BIRD_RADIUS, pillars[i].bottom)) {
                state      = OVER;
                bird.color = RED;
            }
        }
        break;

    case PAUSED:
        if (IsKeyPressed(KEY_P)) {
            state = RUNNING;
        }
        break;

    case OVER:
        if (IsKeyPressed(KEY_SPACE)) {
            init_game();
            state = STANDBY;
        }
        break;
    }

    // handle window resize events
    if (IsWindowResized()) {
        init_game();
    }
}

void draw_game(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    // Game intro
    if (state == STANDBY) {
        DrawText(TextFormat("Press <Space> to start"),
                 SCREEN_WIDTH  / 2.0 - MeasureText("Press <Space> to start", FONT_SIZE) / 2.0,
                 SCREEN_HEIGHT / 2.0 - 20.0 / 2.0 * GetScreenHeight() / 600.0, FONT_SIZE, GRAY);
    } else if (state == PAUSED) {
        DrawText(TextFormat("Paused"),
                 SCREEN_WIDTH  / 2.0 - MeasureText("Paused", FONT_SIZE) / 2.0,
                 SCREEN_HEIGHT / 3.0, FONT_SIZE, GRAY);
    } else if (state == OVER) {
        DrawText(TextFormat("Game over"),
                 SCREEN_WIDTH  / 2.0 - MeasureText("Game over", FONT_SIZE) / 2.0,
                 SCREEN_HEIGHT / 3.0, FONT_SIZE, GRAY);
    }

    // Score
    if (score > 0) {
        DrawText(TextFormat("%d", score),
                 SCREEN_WIDTH  / 2.0 - MeasureText(TextFormat("%d", score), FONT_SIZE * 5.0) / 2.0,
                 SCREEN_HEIGHT / 2.0 - FONT_SIZE * 5.0 / 2.0, FONT_SIZE * 5.0, GRAY);
    }

    // Bird
    DrawEllipse(bird.pos.x, bird.pos.y, bird.radius.x, bird.radius.y, bird.color);

    // Pillars
    for (int i = 0; i < 3; ++i) {
        DrawRectangleRec(pillars[i].top, RAYWHITE);
        DrawRectangleRec(pillars[i].bottom, RAYWHITE);
    }
    EndDrawing();
}

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Flappy Bird");
    SetTargetFPS(FPS);

    SetRandomSeed(time(0));
    init_game();

    while (!WindowShouldClose()) {
        update_game();
        draw_game();
    }

    CloseWindow();
    return 0;
}
