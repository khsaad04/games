#include <math.h>
#include <raylib.h>
#include <raymath.h>

#define SCALE 1.0
#define SCREEN_WIDTH (800 * SCALE)
#define SCREEN_HEIGHT (600 * SCALE)
#define FPS 60
#define PLAYER_WIDTH (100 * SCALE)
#define PLAYER_HEIGHT (20 * SCALE)
#define PLAYER_SPEED (900 * SCALE)
#define BALL_RADIUS (10 * SCALE)
#define BALL_SPEED (300 * SCALE)
#define BALL_ACCEL (3 * SCALE)
#define BRICK_ROWS 10
#define BRICK_COLS 10
#define BRICK_WIDTH (SCREEN_WIDTH / BRICK_COLS - 5 * SCALE)
#define BRICK_HEIGHT (SCREEN_HEIGHT / BRICK_ROWS / 3)
#define FONT_SIZE (20 * SCALE)

typedef enum { STANDBY, RUNNING, PAUSED, OVER } State;

typedef struct {
    Rectangle rect;
    float velocity;
} Player;

typedef struct {
    Vector2 pos, velocity, accel;
    float radius;
} Ball;

typedef struct {
    Rectangle rect;
    bool hit;
} Bricks;

static State game = STANDBY;
static int score = 0;
static Player player = {0};
static Ball ball = {0};
static Bricks bricks[BRICK_ROWS][BRICK_COLS] = {0};

void init_game(void);
void init_bricks(void);
void update_game(void);
void draw_game(void);

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

    player.rect = (Rectangle){.x = SCREEN_WIDTH / 2.0 - PLAYER_WIDTH / 2.0,
                              .y = SCREEN_HEIGHT - SCREEN_HEIGHT * 0.1,
                              .width = PLAYER_WIDTH,
                              .height = PLAYER_HEIGHT};
    player.velocity = PLAYER_SPEED;

    ball.pos = (Vector2){SCREEN_WIDTH / 2.0, 2.0 * SCREEN_HEIGHT / 3.0};
    ball.velocity = (Vector2){BALL_SPEED, -BALL_SPEED};
    ball.accel = (Vector2){BALL_ACCEL, BALL_ACCEL};
    ball.radius = BALL_RADIUS;

    init_bricks();
}

void init_bricks(void)
{
    for (int i = 0; i < BRICK_ROWS; ++i) {
        for (int j = 0; j < BRICK_COLS; ++j) {
            bricks[i][j].rect.x = 2.5 + j * BRICK_WIDTH + j * 5;
            bricks[i][j].rect.y = 2.5 + i * BRICK_HEIGHT + i * 5;
            bricks[i][j].rect.width = BRICK_WIDTH;
            bricks[i][j].rect.height = BRICK_HEIGHT;
            bricks[i][j].hit = false;
        }
    }
}

void update_game(void)
{
    switch (game) {
    case STANDBY:
        // start game
        if (IsKeyPressed(KEY_SPACE)) {
            game = RUNNING;
        }
        break;
    case RUNNING:
        // pause game
        if (IsKeyPressed(KEY_P)) {
            game = PAUSED;
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
        ball.pos =
            Vector2Add(ball.pos, Vector2Scale(ball.velocity, GetFrameTime()));
        ball.velocity =
            Vector2Add(ball.velocity, Vector2Scale(ball.accel, GetFrameTime()));

        // ball-wall collision
        if (ball.pos.x + ball.radius > SCREEN_WIDTH ||
            ball.pos.x < ball.radius) {
            ball.velocity.x *= -1;
        }

        if (ball.pos.y < ball.radius) {
            ball.velocity.y *= -1;
        }

        if (ball.pos.y > SCREEN_HEIGHT + ball.radius) {
            game = OVER;
        }

        // ball-player collision
        if (CheckCollisionCircleRec(ball.pos, ball.radius, player.rect)) {
            ball.pos.y = player.rect.y - ball.radius;
            ball.velocity.y *= -1;

            float ball_dx_from_center =
                ball.pos.x - player.rect.x + player.rect.width / 2;
            float normalized_ball_dx_from_center =
                fminf(1, ball_dx_from_center / (player.rect.width / 2));
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
                                            ball.radius, bricks[i][j].rect)) {
                    if (!bricks[i][j].hit) {
                        score += 1;
                        if (ball.pos.x + ball.radius < bricks[i][j].rect.x ||
                            ball.pos.x - ball.radius >
                                bricks[i][j].rect.x + bricks[i][j].rect.width) {
                            ball.velocity.x *= -1;
                        } else {
                            ball.velocity.y *= -1;
                        }
                        bricks[i][j].hit = true;
                    }
                }
            }
        }

        // reset bricks
        if (fmodf(score, 100) == 0)
            init_bricks();

        break;
    case PAUSED:
        // unpause game
        if (IsKeyPressed(KEY_P)) {
            game = RUNNING;
        }
        break;
    case OVER:
        // restart game
        if (IsKeyPressed(KEY_ENTER)) {
            init_game();
        }
        break;
    }
}

void draw_game(void)
{
    BeginDrawing();

    ClearBackground(BLACK);

    // draw enemy bricks
    for (int i = 0; i < BRICK_ROWS; ++i) {
        for (int j = 0; j < BRICK_COLS; ++j) {
            if (!bricks[i][j].hit) {
                DrawRectangleRec(bricks[i][j].rect, RAYWHITE);
            }
        }
    }

    // draw player tile
    DrawRectangleRec(player.rect, RAYWHITE);

    // draw ball
    DrawCircleV(ball.pos, ball.radius, RAYWHITE);

    const char *text;
    switch (game) {
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
