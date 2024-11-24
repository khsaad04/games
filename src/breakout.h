#ifndef BREAKOUT_H
#define BREAKOUT_H

#include <raylib.h>

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
#define BRICK_WIDTH (SCREEN_WIDTH / BRICK_COLS - 5)
#define BRICK_HEIGHT (SCREEN_HEIGHT / BRICK_ROWS / 3)
#define FONT_SIZE (20 * SCALE)

typedef enum State { STANDBY, RUNNING, PAUSED, OVER } State;

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
    int hit;
} Bricks;

static State game;
static int score;
static Player player;
static Ball ball;
static Bricks bricks[BRICK_ROWS][BRICK_COLS];

void init_game(void);
void init_bricks(void);
void update_game(void);
void draw_game(void);

#endif // BREAKOUT_H
