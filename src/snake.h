#ifndef SNAKE_H_
#define SNAKE_H_

#include <raylib.h>

/*#define CHECKERBOARD*/

#ifdef CHECKERBOARD

#define SNAKE_COLOR BLUE
#define APPLE_COLOR RED
#define BG1 (Color){20, 20, 20, 255}
#define BG2 (Color){10, 10, 10, 255}

#else

#define SNAKE_COLOR RAYWHITE
#define APPLE_COLOR RAYWHITE
#define BG1 BLACK
#define BG2 BLACK

#endif // CHECKERBOARD

#define SCALE 1.0
#define SCREEN_WIDTH (800 * SCALE)
#define SCREEN_HEIGHT (600 * SCALE)
#define FPS 12
#define CELL_SIZE (20 * SCALE)
#define ROWS 30
#define COLS 40
#define SNAKE_CAP (ROWS * COLS)

typedef enum State { STANDBY, RUNNING, PAUSED, OVER } State;

typedef struct {
    Vector2 pos, speed;
} SnakeCell;

typedef struct {
    SnakeCell cells[SNAKE_CAP];
    int len;
} Snake;

typedef struct {
    Vector2 pos;
    float radius;
    bool active;
} Apple;

static State game;
static int score;
static Snake snake;
static Vector2 snake_pos[SNAKE_CAP];
static Apple apple;

void init_game(void);
void update_game(void);
Vector2 get_random_apple_pos(void);
void draw_game(void);

#endif // SNAKE_H_
