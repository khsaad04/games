#include <raylib.h>

static const int scale = 1;
static const int screen_width = (800 * scale);
static const int screen_height = (600 * scale);
static const int fps = 60;
/*static const int rows = 10;*/
/*static const int cols = 10;*/
static const int player_width = (100 * scale);
static const int player_height = (10 * scale);
static const int ball_radius = (10 * scale);
static const int ball_speed = (300 * scale);

enum { rows = 10 }; // NOTE: Did this to fix folding-constant-array warning
enum { cols = 10 }; // NOTE: Did this to fix folding-constant-array warning

typedef enum State { STANDBY, RUNNING, PAUSED, OVER } State;

typedef struct {
    Vector2 pos, size, speed;
} Player;

typedef struct {
    Vector2 pos, speed;
    float radius;
} Ball;

typedef struct {
    Vector2 pos, size;
    bool alive;
} Bricks;

static State game;
static int score;
static Player player;
static Ball ball;
static Bricks bricks[rows][cols];
static int enemy_width;
static int enemy_height;

void init_game(void)
{
    score = 0;
    game = STANDBY;

    int w = GetScreenWidth();
    int h = GetScreenHeight();

    ball.pos = (Vector2){w / 2.0, h - h * 0.1 - ball_radius - player_height};
    ball.radius = ball_radius;
    ball.speed = (Vector2){ball_speed, ball_speed};

    player.pos = (Vector2){w / 2.0 - player_width / 2.0, h - h * 0.1};
    player.size = (Vector2){player_width, player_height};
    player.speed = (Vector2){ball_speed * 2, 0};

    enemy_width = (w - ball.radius * 6 - 5 * (cols - 1)) / cols;
    enemy_height = (h * 0.5 - ball.radius * 3 - 5 * (rows - 1)) / rows;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            bricks[i][j].pos =
                (Vector2){ball.radius * 3 + j * enemy_width + j * 5,
                          ball.radius * 3 + i * enemy_height + i * 5};
            bricks[i][j].size = (Vector2){enemy_width, enemy_height};
            bricks[i][j].alive = true;
        }
    }
}

void update_game(void)
{
    switch (game) {
    case STANDBY:
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) {
            game = RUNNING;
        }
        break;
    case RUNNING:
        if (IsKeyPressed(KEY_P)) {
            game = PAUSED;
        }

        // player control
        if (IsKeyDown(KEY_LEFT)) {
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

        // player-wall collision
        if (player.pos.x > screen_width - player_width) {
            player.pos.x = screen_width - player_width;
        }

        if (player.pos.x < 0) {
            player.pos.x = 0;
        }

        // ball movement
        ball.pos.x += ball.speed.x * GetFrameTime();
        ball.pos.y += ball.speed.y * GetFrameTime();

        // ball-wall collision
        if (ball.pos.x > screen_width || ball.pos.x < 0) {
            ball.speed.x *= -1;
        }

        if (ball.pos.y < 0) {
            ball.speed.y *= -1;
        }

        if (ball.pos.y > screen_height + ball.radius) {
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
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                Rectangle enemy = {
                    .x = bricks[i][j].pos.x,
                    .y = bricks[i][j].pos.y,
                    .width = enemy_width,
                    .height = enemy_height,
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
        if (IsKeyPressed(KEY_P)) {
            game = RUNNING;
        }
        break;
    case OVER:
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
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (bricks[i][j].alive == true) {
                DrawRectangleV(bricks[i][j].pos, bricks[i][j].size, RAYWHITE);
            }
        }
    }
    DrawRectangleV(player.pos, player.size, RAYWHITE);
    DrawCircleV(ball.pos, ball.radius, RAYWHITE);

    int w = GetScreenWidth();
    int h = GetScreenHeight();
    switch (game) {
    case STANDBY:
        DrawText(TextFormat("Press <- or -> to start"),
                 w / 2 - MeasureText("Press <- or -> to start", 20) / 2,
                 h * .75, 20, RAYWHITE);
        break;
    case RUNNING:
        DrawText(TextFormat("Score: %d", score), 20, h - 25, 20, RAYWHITE);
        break;
    case PAUSED:
        DrawText(TextFormat("Press P to resume"),
                 w / 2 - MeasureText("Press P to resume", 30) / 2, h * .55, 30,
                 RAYWHITE);
        break;
    case OVER:
        DrawText(TextFormat("GAME OVER"),
                 w / 2 - MeasureText("GAME OVER", 30) / 2, h * .55, 30,
                 RAYWHITE);
        DrawText(TextFormat("Score: %d", score),
                 w / 2 - MeasureText(TextFormat("Score: %d", score), 30) / 2,
                 h * .65, 30, RAYWHITE);
        DrawText(TextFormat("Press <space> to restart"),
                 w / 2 - MeasureText("Press <space> to restart", 30) / 2,
                 h * .75, 30, RAYWHITE);
        break;
    }

    EndDrawing();
}

int main(void)
{
    InitWindow(screen_width, screen_height, "Breakout");
    SetTargetFPS(fps);

    init_game();

    while (!WindowShouldClose()) {
        update_game();
        draw_game();
    }

    CloseWindow();
    return 0;
}
