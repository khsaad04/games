#include <raylib.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PLAYER_WIDTH 75
#define PLAYER_HEIGHT 15
#define BALL_RADIUS 10
#define BALL_SPEED 300
#define ROWS 10
#define COLS 10

enum State { STANDBY, RUNNING, PAUSED, OVER };

typedef struct Player {
    Vector2 pos;
    Vector2 size;
    Vector2 speed;
    Color color;
} Player;

typedef struct ball {
    Vector2 pos;
    float radius;
    Vector2 speed;
    Color color;
} Ball;

typedef struct Bricks {
    Vector2 pos;
    Vector2 size;
    Color color;
    bool alive;
} Bricks;

static enum State game;
static int score = 0;
static Player player = {0};
static Ball ball = {0};
static Bricks bricks[ROWS][COLS] = {0};

static int enemy_width =
    (SCREEN_WIDTH - BALL_RADIUS * 6 - 5 * (COLS - 1)) / COLS;
static int enemy_height =
    (SCREEN_HEIGHT * 0.5 - BALL_RADIUS * 3 - 5 * (ROWS - 1)) / ROWS;

void init_game(void)
{
    score = 0;
    game = STANDBY;
    ball.pos =
        (Vector2){SCREEN_WIDTH / 2.0, SCREEN_HEIGHT - SCREEN_HEIGHT * 0.1 -
                                          BALL_RADIUS - PLAYER_HEIGHT};
    ball.speed = (Vector2){BALL_SPEED, BALL_SPEED};

    player.pos = (Vector2){SCREEN_WIDTH / 2.0 - PLAYER_WIDTH / 2.0,
                           SCREEN_HEIGHT - SCREEN_HEIGHT * 0.1};
    player.size = (Vector2){PLAYER_WIDTH, PLAYER_HEIGHT};
    player.speed = (Vector2){BALL_SPEED * 2, 0};

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            bricks[i][j].pos =
                (Vector2){BALL_RADIUS * 3 + j * enemy_width + j * 5,
                          BALL_RADIUS * 3 + i * enemy_height + i * 5};
            bricks[i][j].size = (Vector2){enemy_width, enemy_height};
            bricks[i][j].alive = true;
            bricks[i][j].color = RAYWHITE;
        }
    }
}

void update_game(void)
{
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) {
        game = RUNNING;
    }

    if (game == RUNNING) {
        if (IsKeyPressed(KEY_P)) {
            game = PAUSED;
        }
        // player movement
        if (IsKeyDown(KEY_LEFT)) {
            game = RUNNING;
            if (player.speed.x > 0) {
                player.speed.x *= -1;
            }
            player.pos.x += player.speed.x * GetFrameTime();
        }

        if (IsKeyDown(KEY_RIGHT)) {
            game = RUNNING;
            if (player.speed.x < 0) {
                player.speed.x *= -1;
            }
            player.pos.x += player.speed.x * GetFrameTime();
        }

        if (player.pos.x > SCREEN_WIDTH - PLAYER_WIDTH) {
            player.pos.x = SCREEN_WIDTH - PLAYER_WIDTH;
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

        if (ball.pos.y > SCREEN_HEIGHT + BALL_RADIUS) {
            game = OVER;
        }

        // ball-player collision
        Rectangle player_rect = {
            .x = player.pos.x,
            .y = player.pos.y,
            .width = PLAYER_WIDTH,
            .height = PLAYER_HEIGHT,
        };

        // TODO: Fix collisions
        if (CheckCollisionCircleRec(ball.pos, BALL_RADIUS, player_rect)) {
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
                    .width = enemy_width,
                    .height = enemy_height,
                };
                if (CheckCollisionCircleRec((Vector2){ball.pos.x, ball.pos.y},
                                            BALL_RADIUS, enemy)) {
                    if (bricks[i][j].alive == true) {
                        score += 1;
                        ball.speed.y *= -1;
                        bricks[i][j].alive = false;
                    }
                }
            }
        }
    }
    else if (game == PAUSED) {
        if (IsKeyPressed(KEY_P)) {
            game = RUNNING;
        }
    }
    else if (IsKeyPressed(KEY_SPACE)) {
        init_game();
        game = STANDBY;
    }
}

void draw_game(void)
{
    BeginDrawing();

    ClearBackground(BLACK);
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (bricks[i][j].alive == true) {
                DrawRectangleV(bricks[i][j].pos, bricks[i][j].size,
                               bricks[i][j].color);
            }
        }
    }
    DrawRectangleV(player.pos, player.size, RAYWHITE);
    DrawCircleV(ball.pos, BALL_RADIUS, RAYWHITE);

    switch (game) {
    case STANDBY:
        DrawText(TextFormat("Press <- or -> to start"),
                 SCREEN_WIDTH / 2 - MeasureText("Press Enter to start", 20) / 2,
                 SCREEN_HEIGHT * .75, 20, RAYWHITE);
        break;
    case RUNNING:
        DrawText(TextFormat("Score: %d", score), 20, SCREEN_HEIGHT - 25, 20,
                 RAYWHITE);
        break;
    case PAUSED:
        DrawText(TextFormat("Press P to resume"),
                 SCREEN_WIDTH / 2 - MeasureText("Press P to resume", 30) / 2,
                 SCREEN_HEIGHT * .55, 30, RAYWHITE);
        break;
    case OVER:
        DrawText(TextFormat("GAME OVER"),
                 SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 30) / 2,
                 SCREEN_HEIGHT * .55, 30, RAYWHITE);
        DrawText(TextFormat("Score: %d", score),
                 SCREEN_WIDTH / 2 -
                     MeasureText(TextFormat("Score: %d", score), 30) / 2,
                 SCREEN_HEIGHT * .65, 30, RAYWHITE);
        DrawText(
            TextFormat("Press <space> to restart", score),
            SCREEN_WIDTH / 2 -
                MeasureText(TextFormat("Press <space> to restart", score), 30) /
                    2,
            SCREEN_HEIGHT * .75, 30, RAYWHITE);
        break;
    }

    EndDrawing();
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout");
    SetTargetFPS(60);

    init_game();

    while (!WindowShouldClose()) {
        update_game();
        draw_game();
    }

    CloseWindow();
    printf("Score: %d\n", score);
    return 0;
}
