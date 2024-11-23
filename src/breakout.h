#ifndef BREAKOUT_H
#define BREAKOUT_H

#include <raylib.h>

#define SCALE 1.0
#define SCREEN_WIDTH (800 * SCALE)
#define SCREEN_HEIGHT (600 * SCALE)
#define FPS 60
#define ROWS 10
#define COLS 10
#define PLAYER_WIDTH (100 * SCALE)
#define PLAYER_HEIGHT (10 * SCALE)
#define PLAYER_SPEED (600 * SCALE)
#define BALL_RADIUS (10 * SCALE)
#define BALL_SPEED (300 * SCALE)
#define BRICK_WIDTH                                                            \
    ((SCREEN_WIDTH - BALL_RADIUS * 6.0 - 5.0 * (COLS - 1)) / COLS)
#define BRICK_HEIGHT                                                           \
    ((SCREEN_HEIGHT * 0.5 - BALL_RADIUS * 3.0 - 5.0 * (ROWS - 1)) / ROWS)
#define FONT_SIZE (20 * SCALE)

typedef enum State { STANDBY, RUNNING, PAUSED, OVER } State;

typedef struct {
    Vector2 pos, speed, size;
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
static Bricks bricks[ROWS][COLS];

void init_game(void);
void init_bricks(void);
void update_game(void);
void draw_game(void);

#endif // BREAKOUT_H
