#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PLAYER_WIDTH 75
#define PLAYER_HEIGHT 15
#define BALL_RADIUS 10
#define BALL_SPEED 300
#define ROWS 5
#define COLS 10

typedef struct Player {
    Vector2 pos;
    Vector2 size;
    Vector2 speed;
    Color color;
} Player;

typedef struct Ball {
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

static Player player = {0};
static Ball ball = {0};
static Bricks bricks[ROWS][COLS] = {0};

static int enemy_width =
(SCREEN_WIDTH - BALL_RADIUS * 6 - 5 * (COLS - 1)) / COLS;
static int enemy_height = PLAYER_HEIGHT;

void init_game(void) {
    ball.pos = (Vector2){SCREEN_WIDTH / 2.0 - BALL_RADIUS,
        SCREEN_HEIGHT / 2.0 - BALL_RADIUS};
    ball.speed = (Vector2){BALL_SPEED, BALL_SPEED};

    player.pos = (Vector2){SCREEN_WIDTH / 2.0 - PLAYER_WIDTH / 2.0,
        SCREEN_HEIGHT - SCREEN_HEIGHT * 0.1};
    player.size = (Vector2){PLAYER_WIDTH, PLAYER_HEIGHT};
    player.speed = (Vector2){BALL_SPEED * 2, 0};

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            bricks[i][j].pos = (Vector2){BALL_RADIUS * 3 + j * enemy_width + j * 5,
                BALL_RADIUS * 3 + i * enemy_height + i * 5};
            bricks[i][j].size = (Vector2){enemy_width, enemy_height};
            bricks[i][j].alive = true;
            bricks[i][j].color = RAYWHITE;
        }
    }
}

void update_game(void) {
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

    if (ball.pos.y > SCREEN_HEIGHT) {
        CloseWindow();
    }

    // ball-player collision
    Rectangle player_rect = {
        .x = player.pos.x,
        .y = player.pos.y,
        .width = PLAYER_WIDTH,
        .height = PLAYER_HEIGHT,
    };

    if (CheckCollisionCircleRec(ball.pos, BALL_RADIUS, player_rect)) {
        if (player.speed.x < 0 && ball.speed.x > 0) {
            ball.speed.x *= -1;
        }

        if (player.speed.x > 0 && ball.speed.x < 0) {
            ball.speed.x *= -1;
        }

        ball.speed.y *= -1;
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
                    ball.speed.y *= -1;
                    bricks[i][j].alive = false;
                }
            }
        }
    }

    // player movement
    if (IsKeyDown(KEY_LEFT)) {
        if (player.speed.x > 0) {
            player.speed.x *= -1;
        }
        player.pos.x += player.speed.x * GetFrameTime();
    }

    if (IsKeyDown(KEY_RIGHT)) {
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
}

void draw_game(void) {
    BeginDrawing();

    ClearBackground(BLACK);
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (bricks[i][j].alive == true) {
                DrawRectangleV(bricks[i][j].pos, bricks[i][j].size, bricks[i][j].color);
            }
        }
    }
    DrawRectangleV(player.pos, player.size, RAYWHITE);
    DrawCircleV(ball.pos, BALL_RADIUS, RAYWHITE);

    EndDrawing();
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout");
    SetTargetFPS(60);

    init_game();

    while (!WindowShouldClose()) {
        update_game();
        draw_game();
    }

    CloseWindow();
    return 0;
}
