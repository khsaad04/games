#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BAR_WIDTH 100
#define BAR_HEIGHT 10
#define BALL_RADIUS 10
#define BALL_SPEED 250

int main(void) 
{
    float ball_x = SCREEN_WIDTH / 2.0 - BALL_RADIUS;
    float ball_y = SCREEN_HEIGHT / 2.0 - BALL_RADIUS;

    int ball_dx = 1;
    int ball_dy = 1;

    float bar_x = SCREEN_WIDTH / 2.0 - BAR_WIDTH / 2.0;
    float bar_y = SCREEN_HEIGHT - SCREEN_HEIGHT * 0.1;

    int bar_dx = 1;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout");
    SetTargetFPS(60);

    while (!WindowShouldClose()) 
    {
        Rectangle bar = {
            .x = bar_x,
            .y = bar_y,
            .width = BAR_WIDTH,
            .height = BAR_HEIGHT,
        };

        ball_x += BALL_SPEED * GetFrameTime() * ball_dx;
        ball_y += BALL_SPEED * GetFrameTime() * ball_dy;

        // Ball reflection
        if (ball_x > SCREEN_WIDTH || ball_x < 0) {
            ball_dx = -ball_dx;
        }

        if (ball_y > SCREEN_HEIGHT || ball_y < 0) {
            ball_dy = -ball_dy;
        }

        if (CheckCollisionCircleRec((Vector2){ball_x, ball_y}, BALL_RADIUS, bar)) {
            if (bar_dx == -1) {
                ball_dx = -1;
                ball_dy = -ball_dy;
            } else if (bar_dx == 1) {
                ball_dx = 1;
                ball_dy = -ball_dy;
            }
        }

        // Bar movement
        if (IsKeyDown(KEY_LEFT)) {
            bar_dx = -1;
            bar_x += BALL_SPEED * GetFrameTime() * bar_dx;
        }

        if (IsKeyDown(KEY_RIGHT)) {
            bar_dx = 1;
            bar_x += BALL_SPEED * GetFrameTime() * bar_dx;
        }

        if (bar_x > SCREEN_WIDTH - BAR_WIDTH) {
            bar_x = SCREEN_WIDTH - BAR_WIDTH;
        }

        if (bar_x < 0) {
            bar_x = 0;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangle(bar_x, bar_y, BAR_WIDTH, BAR_HEIGHT, RAYWHITE);
        DrawCircle(ball_x, ball_y, BALL_RADIUS, RAYWHITE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
