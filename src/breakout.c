#include <math.h>
#include <raylib.h>
#include <raymath.h>

#define SCALE_X (GetScreenWidth() / 800.0)
#define SCALE_Y (GetScreenHeight() / 600.0)
#define SCREEN_WIDTH (800.0 * SCALE_X)
#define SCREEN_HEIGHT (600.0 * SCALE_Y)
#define FPS 60
#define PLAYER_WIDTH (100.0 * SCALE_X)
#define PLAYER_HEIGHT (20.0 * SCALE_Y)
#define PLAYER_SPEED (900.0 * SCALE_X)
#define BALL_RADIUS (10.0 * SCALE_X)
#define BALL_SPEED (300.0 * SCALE_X)
#define BALL_ACCEL (3.0 * SCALE_X)
#define BRICK_ROWS 10
#define BRICK_COLS 10
#define BRICK_PADDING (5.0 * SCALE_X)
#define BRICK_WIDTH (SCREEN_WIDTH / BRICK_COLS - BRICK_PADDING)
#define BRICK_HEIGHT (SCREEN_HEIGHT / BRICK_ROWS / 3.0)
#define FONT_SIZE (20.0 * SCALE_Y)

typedef enum { STANDBY, RUNNING, PAUSED, OVER } State;

typedef struct {
    Rectangle rect;
    float     velocity;
} Player;

typedef struct {
    Vector2   pos, velocity, accel;
    float     radius;
    bool      alive;
} Ball;

typedef struct {
    Rectangle rect [BRICK_ROWS][BRICK_COLS];
    bool      hit  [BRICK_ROWS][BRICK_COLS];
} Bricks;

State  state;
int    score;
Player player;
Ball   ball;
Bricks bricks;

void init_bricks(void)
{
    for (int i = 0; i < BRICK_ROWS; ++i) {
        for (int j = 0; j < BRICK_COLS; ++j) {
            bricks.rect[i][j].x      = BRICK_PADDING * 0.5 + j * BRICK_WIDTH  + j * BRICK_PADDING;
            bricks.rect[i][j].y      = BRICK_PADDING * 0.5 + i * BRICK_HEIGHT + i * BRICK_PADDING;
            bricks.rect[i][j].width  = BRICK_WIDTH;
            bricks.rect[i][j].height = BRICK_HEIGHT;
            bricks.hit[i][j]         = false;
        }
    }
}

void init_game(void)
{
    score = 0;
    state = STANDBY;

    player.rect = (Rectangle){.x      = SCREEN_WIDTH / 2.0 - PLAYER_WIDTH / 2.0,
                              .y      = SCREEN_HEIGHT - SCREEN_HEIGHT * 0.1,
                              .width  = PLAYER_WIDTH,
                              .height = PLAYER_HEIGHT};
    player.velocity = PLAYER_SPEED;

    ball.pos      = (Vector2){SCREEN_WIDTH / 2.0, 2.0 * SCREEN_HEIGHT / 3.0};
    ball.velocity = (Vector2){BALL_SPEED, -BALL_SPEED};
    ball.accel    = (Vector2){BALL_ACCEL, BALL_ACCEL};
    ball.radius   = BALL_RADIUS;
    ball.alive    = true;

    init_bricks();
}

void update_game(void)
{
    switch (state) {
    case STANDBY:
        // start game
        if (IsKeyPressed(KEY_SPACE)) {
            state = RUNNING;
        }
        break;
    case RUNNING:
        // pause game
        if (IsKeyPressed(KEY_P)) {
            state = PAUSED;
        }

        // player control
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            player.velocity = -fabsf(player.velocity);
            player.rect.x += player.velocity * GetFrameTime();
        }

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            player.velocity = fabsf(player.velocity);
            player.rect.x += player.velocity * GetFrameTime();
        }

        // player-wall collision
        if (player.rect.x > SCREEN_WIDTH - player.rect.width) {
            player.rect.x = SCREEN_WIDTH - player.rect.width;
        }

        if (player.rect.x < 0) {
            player.rect.x = 0;
        }

        // ball movement
        ball.pos      = Vector2Add(ball.pos,      Vector2Scale(ball.velocity, GetFrameTime()));
        ball.velocity = Vector2Add(ball.velocity, Vector2Scale(ball.accel,    GetFrameTime()));

        // ball-wall collision
        if (ball.pos.x + ball.radius > SCREEN_WIDTH ||
            ball.pos.x < ball.radius) {
            ball.velocity.x *= -1;
        }

        if (ball.pos.y < ball.radius) {
            ball.velocity.y *= -1;
        }

        if (ball.pos.y > SCREEN_HEIGHT + ball.radius) {
            ball.alive = false;
            state      = OVER;
        }

        // ball-player collision
        if (CheckCollisionCircleRec(ball.pos, ball.radius, player.rect)) {
            ball.pos.y = player.rect.y - ball.radius;
            ball.velocity.y *= -1;

            float ball_dx_from_center = ball.pos.x - player.rect.x + player.rect.width / 2;
            float normalized_ball_dx_from_center = fminf(1, ball_dx_from_center / (player.rect.width / 2));
            ball.velocity.x = BALL_SPEED * normalized_ball_dx_from_center;

            if (player.velocity < 0 && ball.velocity.x > 0) {
                ball.velocity.x *= -1;
            }
            if (player.velocity > 0 && ball.velocity.x < 0) {
                ball.velocity.x *= -1;
            }
        }

        // ball-brick collision
        for (int i = 0; i < BRICK_ROWS; ++i) {
            for (int j = 0; j < BRICK_COLS; ++j) {
                if (CheckCollisionCircleRec((Vector2){ball.pos.x, ball.pos.y},
                                            ball.radius, bricks.rect[i][j])) {
                    if (!bricks.hit[i][j]) {
                        score += 1;
                        if (ball.pos.x + ball.radius < bricks.rect[i][j].x ||
                            ball.pos.x - ball.radius >
                                bricks.rect[i][j].x + bricks.rect[i][j].width) {
                            ball.velocity.x *= -1;
                        } else {
                            ball.velocity.y *= -1;
                        }
                        bricks.hit[i][j] = true;
                    }
                }
            }
        }

        // reset bricks
        if (fmodf(score, 100) == 0) init_bricks();

        break;
    case PAUSED:
        // unpause game
        if (IsKeyPressed(KEY_P)) {
            state = RUNNING;
        }
        break;
    case OVER:
        // restart game
        if (IsKeyPressed(KEY_ENTER)) {
            init_game();
        }
        break;
    }

    // handle window resize events
    if (IsWindowResized()) {
        player.rect.x      = SCREEN_WIDTH / 2.0 - PLAYER_WIDTH / 2.0;
        player.rect.y      = SCREEN_HEIGHT - SCREEN_HEIGHT * 0.1;
        player.rect.width  = PLAYER_WIDTH;
        player.rect.height = PLAYER_HEIGHT;
        player.velocity    = PLAYER_SPEED;

        ball.pos      = (Vector2){SCREEN_WIDTH / 2.0, 2.0 * SCREEN_HEIGHT / 3.0};
        ball.radius   = BALL_RADIUS;
        ball.velocity = (Vector2){BALL_SPEED, -BALL_SPEED};
        ball.accel    = (Vector2){BALL_ACCEL, BALL_ACCEL};

        for (int i = 0; i < BRICK_ROWS; ++i) {
            for (int j = 0; j < BRICK_COLS; ++j) {
                bricks.rect[i][j].x      = BRICK_PADDING * 0.5 + j * BRICK_WIDTH  + j * BRICK_PADDING;
                bricks.rect[i][j].y      = BRICK_PADDING * 0.5 + i * BRICK_HEIGHT + i * BRICK_PADDING;
                bricks.rect[i][j].width  = BRICK_WIDTH;
                bricks.rect[i][j].height = BRICK_HEIGHT;
            }
        }
    }
}

void draw_game(void)
{
    BeginDrawing();

    ClearBackground(BLACK);

    // draw enemy bricks
    for (int i = 0; i < BRICK_ROWS; ++i) {
        for (int j = 0; j < BRICK_COLS; ++j) {
            if (!bricks.hit[i][j]) {
                DrawRectangleRec(bricks.rect[i][j], RAYWHITE);
            }
        }
    }

    // draw player tile
    DrawRectangleRec(player.rect, RAYWHITE);

    // draw ball
    if (ball.alive) DrawCircleV(ball.pos, ball.radius, RAYWHITE);

    const char *text;
    switch (state) {
    case STANDBY:
        text = "Press <space> to start";
        DrawText(text, SCREEN_WIDTH / 2 - MeasureText(text, FONT_SIZE) / 2.0,
                 SCREEN_HEIGHT * .75, FONT_SIZE, RAYWHITE);
        break;

    case RUNNING:
        DrawText(TextFormat("Score: %d", score), 20, SCREEN_HEIGHT - 25,
                 FONT_SIZE, RAYWHITE);
        break;

    case PAUSED:
        text = "Paused";
        DrawText(text,
                 SCREEN_WIDTH / 2 - MeasureText(text, FONT_SIZE * 3 / 2) / 2.0,
                 SCREEN_HEIGHT * .55, FONT_SIZE * 3 / 2, RAYWHITE);
        break;

    case OVER:
        text = "GAME OVER";
        DrawText(text,
                 SCREEN_WIDTH / 2 - MeasureText(text, FONT_SIZE * 3 / 2) / 2.0,
                 SCREEN_HEIGHT * .55, FONT_SIZE * 3 / 2, RAYWHITE);
        text = TextFormat("Score: %d", score);
        DrawText(text,
                 SCREEN_WIDTH / 2 - MeasureText(text, FONT_SIZE * 3 / 2) / 2.0,
                 SCREEN_HEIGHT * .65, FONT_SIZE * 3 / 2, RAYWHITE);
        text = "Press <enter> to restart";
        DrawText(text,
                 SCREEN_WIDTH / 2 - MeasureText(text, FONT_SIZE * 3 / 2) / 2.0,
                 SCREEN_HEIGHT * .75, FONT_SIZE * 3 / 2, RAYWHITE);
        break;
    }

    EndDrawing();
}

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Breakout");
    SetTargetFPS(FPS);

    init_game();

    while (!WindowShouldClose()) {
        update_game();
        draw_game();
    }

    CloseWindow();
    return 0;
}
