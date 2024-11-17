#include "breakout.h"
#include <raylib.h>

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

void init_game(void)
{
    score = 0;
    game = STANDBY;

    int w = (int)SCREEN_WIDTH;
    int h = (int)SCREEN_HEIGHT;

    player.pos = (Vector2){w / 2.0 - PLAYER_WIDTH / 2.0, h - h * 0.1};
    player.speed = (Vector2){PLAYER_SPEED, 0};
    player.size = (Vector2){PLAYER_WIDTH, PLAYER_HEIGHT};

    ball.pos = (Vector2){w / 2.0, h - w * 0.1 - BALL_RADIUS - player.size.y};
    ball.speed = (Vector2){BALL_SPEED, BALL_SPEED};
    ball.radius = BALL_RADIUS;

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            bricks[i][j].pos =
                (Vector2){ball.radius * 3 + j * BRICK_WIDTH + j * 5,
                          ball.radius * 3 + i * BRICK_HEIGHT + i * 5};
            bricks[i][j].size = (Vector2){BRICK_WIDTH, BRICK_HEIGHT};
            bricks[i][j].alive = true;
        }
    }
}

void update_game(void)
{
    switch (game) {
    case STANDBY:
        // start game
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) || IsKeyDown(KEY_RIGHT) ||
            IsKeyDown(KEY_D)) {
            game = RUNNING;
        }
        break;
    case RUNNING:
        // pause game
        if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_SPACE)) {
            game = PAUSED;
        }

        // player control
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            if (player.speed.x > 0) {
                player.speed.x *= -1;
            }
            player.pos.x += player.speed.x * GetFrameTime();
        }

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            game = RUNNING;
            if (player.speed.x < 0) {
                player.speed.x *= -1;
            }
            player.pos.x += player.speed.x * GetFrameTime();
        }

        // player-wall collision
        if (player.pos.x > SCREEN_WIDTH - player.size.x) {
            player.pos.x = SCREEN_WIDTH - player.size.x;
        }

        if (player.pos.x < 0) {
            player.pos.x = 0;
        }

        // ball movement
        ball.pos.x += ball.speed.x * GetFrameTime();
        ball.pos.y += ball.speed.y * GetFrameTime();

        // ball-wall collision
        if (ball.pos.x > SCREEN_WIDTH || ball.pos.x < 0) {
            ball.speed.x *= -1;
        }

        if (ball.pos.y < 0) {
            ball.speed.y *= -1;
        }

        if (ball.pos.y > SCREEN_HEIGHT + ball.radius) {
            game = OVER;
        }

        // ball-player collision
        Rectangle player_rect = {
            .x = player.pos.x,
            .y = player.pos.y,
            .width = player.size.x,
            .height = player.size.y,
        };

        if (CheckCollisionCircleRec(ball.pos, ball.radius, player_rect)) {
            ball.pos.y = player.pos.y - ball.radius;
            ball.speed.y *= -1;
            if (player.speed.x < 0 && ball.speed.x > 0) {
                ball.speed.x *= -1;
            }
            if (player.speed.x > 0 && ball.speed.x < 0) {
                ball.speed.x *= -1;
            }
        }

        // ball-brick collision
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                Rectangle enemy = {
                    .x = bricks[i][j].pos.x,
                    .y = bricks[i][j].pos.y,
                    .width = bricks[i][j].size.x,
                    .height = bricks[i][j].size.y,
                };
                if (CheckCollisionCircleRec((Vector2){ball.pos.x, ball.pos.y},
                                            ball.radius, enemy)) {
                    if (bricks[i][j].alive == true) {
                        score += 1;
                        ball.speed.y *= -1;
                        bricks[i][j].alive = false;
                    }
                }
            }
        }
        break;
    case PAUSED:
        // unpause game
        if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_SPACE)) {
            game = RUNNING;
        }
        break;
    case OVER:
        // restart game
        if (IsKeyPressed(KEY_SPACE)) {
            init_game();
        }
        break;
    }
}

void draw_game(void)
{
    BeginDrawing();

    ClearBackground(BLACK);
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (bricks[i][j].alive == true) {
                DrawRectangleV(bricks[i][j].pos, bricks[i][j].size, RAYWHITE);
            }
        }
    }
    DrawRectangleV(player.pos, (Vector2){player.size.x, player.size.y},
                   RAYWHITE);
    DrawCircleV(ball.pos, ball.radius, RAYWHITE);

    int w = (int)SCREEN_WIDTH;
    int h = (int)SCREEN_HEIGHT;

    const char *text;
    switch (game) {
    case STANDBY:
        text = "Press <-/A or ->/D to start";
        DrawText(text, w / 2 - MeasureText(text, FONT_SIZE) / 2, h * .75,
                 FONT_SIZE, RAYWHITE);
        break;
    case RUNNING:
        DrawText(TextFormat("Score: %d", score), 20, h - 25, FONT_SIZE,
                 RAYWHITE);
        break;
    case PAUSED:
        text = "Press P/<space> to resume";
        DrawText(text, w / 2 - MeasureText(text, FONT_SIZE * 3 / 2) / 2,
                 h * .55, FONT_SIZE * 3 / 2, RAYWHITE);
        break;
    case OVER:
        text = "GAME OVER";
        DrawText(text, w / 2 - MeasureText(text, FONT_SIZE * 3 / 2) / 2,
                 h * .55, FONT_SIZE * 3 / 2, RAYWHITE);
        text = TextFormat("Score: %d", score);
        DrawText(text, w / 2 - MeasureText(text, FONT_SIZE * 3 / 2) / 2,
                 h * .65, FONT_SIZE * 3 / 2, RAYWHITE);
        text = "Press <space> to restart";
        DrawText(text, w / 2 - MeasureText(text, FONT_SIZE * 3 / 2) / 2,
                 h * .75, FONT_SIZE * 3 / 2, RAYWHITE);
        break;
    }

    EndDrawing();
}
