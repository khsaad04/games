#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define FPS 60

#define SCALE_X (box_size.x / SCREEN_WIDTH)
#define SCALE_Y (box_size.y / SCREEN_HEIGHT)

#define PADDLE_WIDTH (100 * SCALE_X)
#define PADDLE_HEIGHT (20 * SCALE_Y)
#define PADDLE_SPEED (900 * SCALE_X)

#define BALL_RADIUS (10 * SCALE_X)
#define BALL_SPEED (300 * SCALE_X)

#define BRICK_ROWS 10
#define BRICK_COLS 10
#define BRICK_PADDING (5 * SCALE_X)
#define BRICK_WIDTH (box_size.x / BRICK_COLS - BRICK_PADDING)
#define BRICK_HEIGHT (box_size.y / BRICK_ROWS / 3)

#define BG_COLOR ((Color){0x14, 0x14, 0x0C, 0xFF})
#define FG_COLOR ((Color){0xE6, 0xE3, 0xD5, 0xFF})
#define FONT_SIZE (20 * SCALE_X)

typedef struct {
    Rectangle rect;
    float     speed;
} Paddle;

typedef struct {
    Vector2   pos, speed;
    float     radius;
    bool      active;
} Ball;

typedef struct {
    Rectangle rect[BRICK_ROWS][BRICK_COLS];
    bool      hit[BRICK_ROWS][BRICK_COLS];
} Bricks;

static Paddle paddle    = {0};
static Ball   ball      = {0};
static Bricks bricks    = {0};
static int    score     =  0;
static bool   pause     = true;
static bool   game_over = false;

static Vector2 box_origin = {0};
static Vector2 box_size   = {SCREEN_WIDTH, SCREEN_HEIGHT};

static inline void init_bricks(void)
{
    for (int i = 0; i < BRICK_ROWS; ++i) {
        for (int j = 0; j < BRICK_COLS; ++j) {
            bricks.rect[i][j].x      = box_origin.x + BRICK_PADDING * 0.5 + j * BRICK_WIDTH  + j * BRICK_PADDING;
            bricks.rect[i][j].y      = box_origin.y + BRICK_PADDING * 0.5 + i * BRICK_HEIGHT + i * BRICK_PADDING;
            bricks.rect[i][j].width  = BRICK_WIDTH;
            bricks.rect[i][j].height = BRICK_HEIGHT;
            bricks.hit[i][j]         = false;
        }
    }
}

static inline void init_game(void)
{
    score = 0;

    float paddle_x = box_origin.x + box_size.x * 0.5 - PADDLE_WIDTH * 0.5;
    float paddle_y = box_origin.y + box_size.y * 0.9;
    paddle.rect    = (Rectangle){.x      = paddle_x,
                                 .y      = paddle_y,
                                 .width  = PADDLE_WIDTH,
                                 .height = PADDLE_HEIGHT};
    paddle.speed   = PADDLE_SPEED;

    ball.pos    = (Vector2){paddle_x + PADDLE_WIDTH * 0.5, paddle_y - BALL_RADIUS * 2.0};
    ball.speed  = (Vector2){BALL_SPEED, BALL_SPEED};
    ball.radius = BALL_RADIUS;
    ball.active = true;

    init_bricks();
}

static inline void update_game(void)
{
    if (IsWindowResized()) {
        int new_screen_width  = GetScreenWidth();
        int new_screen_height = GetScreenHeight();

        if (new_screen_width / 16 >= new_screen_height / 9) {
            box_size.y = new_screen_height;
            box_size.x = box_size.y * 16 / 9;
        } else {
            box_size.x = new_screen_width;
            box_size.y = box_size.x * 9  / 16;
        }

        box_origin.x = (new_screen_width  - box_size.x) * 0.5;
        box_origin.y = (new_screen_height - box_size.y) * 0.5;

        float paddle_x = box_origin.x + box_size.x * 0.5 - PADDLE_WIDTH * 0.5;
        float paddle_y = box_origin.y + box_size.y * 0.9;
        paddle.rect    = (Rectangle){.x      = paddle_x,
                                     .y      = paddle_y,
                                     .width  = PADDLE_WIDTH,
                                     .height = PADDLE_HEIGHT};
        paddle.speed   = PADDLE_SPEED;

        ball.pos    = (Vector2){paddle_x + PADDLE_WIDTH * 0.5, paddle_y - BALL_RADIUS * 2.0};
        ball.speed  = (Vector2){BALL_SPEED, BALL_SPEED};
        ball.radius = BALL_RADIUS;

        for (int i = 0; i < BRICK_ROWS; ++i) {
            for (int j = 0; j < BRICK_COLS; ++j) {
                bricks.rect[i][j].x      = box_origin.x + BRICK_PADDING * 0.5 + j * BRICK_WIDTH + j * BRICK_PADDING;
                bricks.rect[i][j].y      = box_origin.y + BRICK_PADDING * 0.5 + i * BRICK_HEIGHT + i * BRICK_PADDING;
                bricks.rect[i][j].width  = BRICK_WIDTH;
                bricks.rect[i][j].height = BRICK_HEIGHT;
            }
        }
    }

    if (!game_over) {
        if (!pause) {
            // toggle pause
            if (IsKeyPressed(KEY_SPACE)) pause = !pause;

            // paddle control
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
                paddle.speed = -fabsf(paddle.speed);
                paddle.rect.x += paddle.speed * GetFrameTime();
            }

            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
                paddle.speed = fabsf(paddle.speed);
                paddle.rect.x += paddle.speed * GetFrameTime();
            }

            // paddle-wall collision
            if (paddle.rect.x > box_size.x + box_origin.x - paddle.rect.width) {
                paddle.rect.x = box_size.x + box_origin.x - paddle.rect.width;
            }

            if (paddle.rect.x < box_origin.x) {
                paddle.rect.x = box_origin.x;
            }

            // ball movement
            ball.pos = Vector2Add(ball.pos, Vector2Scale(ball.speed, GetFrameTime()));

            // ball-wall collision
            if (ball.pos.x + ball.radius > box_size.x + box_origin.x ||
                ball.pos.x < ball.radius + box_origin.x) {
                ball.speed.x *= -1;
            }

            if (ball.pos.y < ball.radius + box_origin.y) {
                ball.speed.y *= -1;
            }

            if (ball.pos.y > box_size.y + box_origin.y + ball.radius) {
                ball.active = false;
                game_over   = true;
            }

            // ball-paddle collision
            if (CheckCollisionCircleRec(ball.pos, ball.radius, paddle.rect)) {
                ball.pos.y = paddle.rect.y - ball.radius;
                ball.speed.y *= -1;

                if (paddle.speed < 0 && ball.speed.x > 0) {
                    ball.speed.x *= -1;
                }
                if (paddle.speed > 0 && ball.speed.x < 0) {
                    ball.speed.x *= -1;
                }
            }

            // ball-brick collision
            for (int i = 0; i < BRICK_ROWS; ++i) {
                for (int j = 0; j < BRICK_COLS; ++j) {
                    if (CheckCollisionCircleRec((Vector2){ball.pos.x, ball.pos.y}, ball.radius, bricks.rect[i][j])) {
                        if (!bricks.hit[i][j]) {
                            score += 1;
                            if (ball.pos.x + ball.radius < bricks.rect[i][j].x ||
                                ball.pos.x - ball.radius > bricks.rect[i][j].x + bricks.rect[i][j].width) {
                                ball.speed.x *= -1;
                            } else {
                                ball.speed.y *= -1;
                            }
                            bricks.hit[i][j] = true;
                        }
                    }
                }
            }

            // reset bricks
            if (fmodf(score, 100) == 0) init_bricks();
        } else {
            // toggle pause
            if (IsKeyPressed(KEY_SPACE)) pause = !pause;
        }
    } else {
        // restart game
        if (IsKeyPressed(KEY_ENTER)) {
            game_over = !game_over;
            init_game();
            pause = true;
        }
    }
}

static inline void draw_game(void)
{
    BeginDrawing();
    {
        ClearBackground(BLACK);
        DrawRectangleV(box_origin, box_size, BG_COLOR);

        // draw enemy bricks
        for (int i = 0; i < BRICK_ROWS; ++i) {
            for (int j = 0; j < BRICK_COLS; ++j) {
                if (!bricks.hit[i][j]) {
                    DrawRectangleRec(bricks.rect[i][j], FG_COLOR);
                }
            }
        }

        // draw paddle tile
        DrawRectangleRec(paddle.rect, FG_COLOR);

        // draw ball
        if (ball.active) DrawCircleV(ball.pos, ball.radius, FG_COLOR);

        const char *text = TextFormat("Score: %d", score);
        DrawText(text, box_origin.x + 20.0 * SCALE_X, box_origin.y + box_size.y - 25.0 * SCALE_Y, FONT_SIZE, FG_COLOR);

        if (pause) {
            text = "Press <space> to start";
            DrawText(text,
                     box_origin.x + box_size.x * 0.5 - MeasureText(text, FONT_SIZE) / 2.0,
                     box_origin.y + box_size.y * 0.75, FONT_SIZE, FG_COLOR);
        }

        if (game_over) {
            text = "GAME OVER";
            DrawText(text,
                     box_origin.x + box_size.x * 0.5 - MeasureText(text, FONT_SIZE * 1.5) / 2.0,
                     box_origin.y + box_size.y * 0.55, FONT_SIZE * 1.5, FG_COLOR);
            text = "Press <enter> to restart";
            DrawText(text,
                     box_origin.x + box_size.x * 0.5 - MeasureText(text, FONT_SIZE * 1.5) / 2.0,
                     box_origin.y + box_size.y * 0.65, FONT_SIZE * 1.5, FG_COLOR);
        }
    }
    EndDrawing();
}

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout [Rewrite]");
    SetTargetFPS(FPS);

    init_game();

    while (!WindowShouldClose()) {
        update_game();
        draw_game();
    }

    return 0;
}
