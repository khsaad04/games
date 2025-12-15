#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define FPS 60
#define RANDOM_SEED 2147483629

#define SCALE_X (box.width / SCREEN_WIDTH)
#define SCALE_Y (box.height / SCREEN_HEIGHT)

#define BIRD_RADIUS (20 * SCALE_Y)
#define BIRD_GRAVITY (1500 * SCALE_Y)
#define BIRD_FLAP (-BIRD_GRAVITY / 4)

#define PILLARS_COUNT 5
#define PILLAR_WIDTH (BIRD_RADIUS * 3)
#define PILLAR_GAP (BIRD_RADIUS * 8)
#define PILLAR_PADDING (PILLAR_WIDTH * 3.5)
#define PILLAR_SPEED (150 * SCALE_X)
#define PILLAR_ACCEL (1.5 * SCALE_X)

#define BG_COLOR ((Color){0x14, 0x14, 0x0C, 0xFF})
#define FG_COLOR ((Color){0xE6, 0xE3, 0xD5, 0xFF})
#define FONT_SIZE (20 * SCALE_Y)

typedef struct {
    Vector2   pos, radius;
    float     speed;
    Color     color;
} Bird;

typedef struct {
    Rectangle top, bottom;
    float     speed, accel;
    bool      passed;
} Pillars;

int     score = 0;
Bird    bird  = {0};
Pillars pillars[PILLARS_COUNT] = {0};
bool    pause     = true;
bool    game_over = false;

Rectangle box = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

float random_piller_y(void)
{
    return GetRandomValue(box.height * 0.1, box.height * 0.9 - PILLAR_GAP);
}

void init_game(void)
{
    SetRandomSeed(RANDOM_SEED);
    score = 0;

    bird.pos    = (Vector2){box.x + box.width / 3, box.y + box.height * 2 / 5};
    bird.radius = (Vector2){BIRD_RADIUS, BIRD_RADIUS};
    bird.color  = RAYWHITE;
    bird.speed  = BIRD_FLAP;

    for (int i = 0; i < PILLARS_COUNT; ++i) {
        float y_cord = random_piller_y();

        pillars[i].top.x      = pillars[i].bottom.x = box.x + box.width * 2 / 3 + i * PILLAR_PADDING;
        pillars[i].top.y      = box.y;
        pillars[i].top.width  = pillars[i].bottom.width = PILLAR_WIDTH;
        pillars[i].top.height = y_cord;

        pillars[i].bottom.y      = box.y + y_cord + PILLAR_GAP;
        pillars[i].bottom.height = box.height - y_cord - PILLAR_GAP;

        pillars[i].passed = false;

        pillars[i].accel = PILLAR_ACCEL;
        pillars[i].speed = PILLAR_SPEED;
    }
}

void update_game(void)
{
    if (IsWindowResized()) {
        float bird_ratio_x = (bird.pos.x - box.x) / box.width;
        float bird_ratio_y = (bird.pos.y - box.y) / box.height;

        Pillars pillars_ratio[PILLARS_COUNT] = {0};
        for (int i = 0; i < PILLARS_COUNT; ++i) {
            pillars_ratio[i].top.x      = (pillars[i].top.x - box.x) / box.width;
            pillars_ratio[i].top.height = pillars[i].top.height / box.height;

            pillars_ratio[i].bottom.x      = (pillars[i].bottom.x - box.x) / box.width;
            pillars_ratio[i].bottom.y      = (pillars[i].bottom.y - box.y) / box.height;
            pillars_ratio[i].bottom.height = pillars[i].bottom.height / box.height;
        }

        int new_screen_width  = GetScreenWidth();
        int new_screen_height = GetScreenHeight();

        if (new_screen_width / 16 >= new_screen_height / 9) {
            box.height = new_screen_height;
            box.width  = box.height * 16 / 9;
        } else {
            box.width  = new_screen_width;
            box.height = box.width * 9 / 16;
        }

        box.x = (new_screen_width  - box.width)  * 0.5;
        box.y = (new_screen_height - box.height) * 0.5;

        bird.pos    = (Vector2){box.x + box.width * bird_ratio_x, box.y + box.height * bird_ratio_y};
        bird.radius = (Vector2){BIRD_RADIUS, BIRD_RADIUS};
        bird.speed  = BIRD_FLAP;

        for (int i = 0; i < PILLARS_COUNT; ++i) {
            pillars[i].top.x      = box.x + box.width * pillars_ratio[i].top.x;
            pillars[i].top.y      = box.y;
            pillars[i].top.width  = pillars[i].bottom.width = PILLAR_WIDTH;
            pillars[i].top.height = box.height * pillars_ratio[i].top.height;

            pillars[i].bottom.x      = box.x + box.width * pillars_ratio[i].bottom.x;
            pillars[i].bottom.y      = box.y + box.height * pillars_ratio[i].bottom.y;
            pillars[i].bottom.height = box.height * pillars_ratio[i].bottom.height;

            pillars[i].accel = PILLAR_ACCEL;
            pillars[i].speed = PILLAR_SPEED;
        }
    }

    if (!game_over) {
        if (!pause) {
            if (IsKeyPressed(KEY_P)) pause = true;

            if (IsKeyPressed(KEY_SPACE)) bird.speed = BIRD_FLAP;

            bird.pos.y    += bird.speed * GetFrameTime();
            bird.radius.y -= bird.speed * 0.02 * GetFrameTime();
            bird.radius.x += bird.speed * 0.02 * GetFrameTime();
            bird.speed    += BIRD_GRAVITY * GetFrameTime();

            if (bird.pos.y < box.y - BIRD_RADIUS ||
                bird.pos.y > box.y + box.height + BIRD_RADIUS) {
                game_over  = true;
                bird.color = RED;
            }

            for (int i = 0; i < PILLARS_COUNT; ++i) {
                pillars[i].top.x = pillars[i].bottom.x -= pillars[i].speed * GetFrameTime();
                pillars[i].speed += pillars[i].accel * GetFrameTime();

                if (pillars[i].top.x < box.x - PILLAR_WIDTH) {
                    float y_cord = random_piller_y();
                    pillars[i].top.x      = pillars[i].bottom.x += (PILLAR_WIDTH + PILLAR_PADDING) * (PILLARS_COUNT - 1);
                    pillars[i].top.y      = box.y;
                    pillars[i].top.height = y_cord;

                    pillars[i].bottom.y      = box.y + y_cord + PILLAR_GAP;
                    pillars[i].bottom.height = box.height - y_cord - PILLAR_GAP;
                    pillars[i].passed        = false;
                }

                if (pillars[i].top.x <= bird.pos.x) {
                    if (!pillars[i].passed) {
                        score += 1;
                        pillars[i].passed = true;
                    }
                }

                if (CheckCollisionCircleRec(bird.pos, BIRD_RADIUS, pillars[i].top) ||
                    CheckCollisionCircleRec(bird.pos, BIRD_RADIUS, pillars[i].bottom)) {
                    game_over  = true;
                    bird.color = RED;
                }
            }
        } else {
            if (IsKeyPressed(KEY_SPACE)) pause = false;
        }
    } else {
        if (IsKeyPressed(KEY_ENTER)) {
            game_over = false;
            pause     = true;

            init_game();
        }
    }
}

void draw_game(void)
{
    BeginDrawing();
    {
        ClearBackground(BLACK);
        DrawRectangleRec(box, BG_COLOR);

        const char *text = TextFormat("%d", score);
        if (score > 0) {
            DrawText(text,
                     box.x + box.width  * 0.5 - MeasureText(text, FONT_SIZE * 5) * 0.5,
                     box.y + box.height * 0.5 - FONT_SIZE * 2.5, FONT_SIZE * 5,
                     GRAY);
        }

        // Bird
        DrawEllipse(bird.pos.x, bird.pos.y, bird.radius.x, bird.radius.y, bird.color);

        // Pillars
        for (int i = 0; i < PILLARS_COUNT; ++i) {
            DrawRectangleRec(pillars[i].top,    RAYWHITE);
            DrawRectangleRec(pillars[i].bottom, RAYWHITE);
        }

        text = "Press <Space> to start";
        if (pause) {
            DrawText(text,
                     box.x + box.width  * 0.5 - MeasureText(text, FONT_SIZE) * 0.5,
                     box.y + box.height * 0.8, FONT_SIZE, GRAY);
        }

        if (game_over) {
            text = "Game over";
            DrawText(text,
                     box.x + box.width * 0.5 - MeasureText(text, FONT_SIZE * 2) * 0.5,
                     box.y + box.height / 4, FONT_SIZE * 2, GRAY);

            text = "Press <Enter> to restart";
            DrawText(text,
                     box.x + box.width  * 0.5 - MeasureText(text, FONT_SIZE) * 0.5,
                     box.y + box.height * 0.8, FONT_SIZE, GRAY);
        }

        DrawRectangle(0, 0, box.x, box.height, BLACK);
        DrawRectangle(box.x + box.width, 0, box.x * 2, box.height, BLACK);
    }
    EndDrawing();
}

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Flappy bird");
    SetTargetFPS(FPS);

    init_game();

    while (!WindowShouldClose()) {
        update_game();
        draw_game();
    }
    CloseWindow();
    return 0;
}
