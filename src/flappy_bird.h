#ifndef FLAPPY_BIRD_H
#define FLAPPY_BIRD_H

#include <raylib.h>

#define SCALE 1.0
#define SCREEN_WIDTH (800 * SCALE)
#define SCREEN_HEIGHT (600 * SCALE)
#define FPS 60
#define BIRD_RADIUS (20 * SCALE)
#define GRAVITY (30 * SCALE)
#define PILLAR_WIDTH (5 * BIRD_RADIUS)
#define PILLAR_GAP (10 * BIRD_RADIUS)
#define PILLAR_PADDING ((SCREEN_WIDTH - 2 * PILLAR_WIDTH) / 2)

typedef enum { STANDBY, RUNNING, PAUSED, OVER } State;

typedef struct {
    Vector2 pos, radius;
    Color color;
} Bird;

typedef struct {
    Vector2 pos;
    bool passed;
} Pillar;

static State game;
static int score;
static Bird bird;
static float bird_velocity;
static Pillar pillars[3];
static float pillar_velocity;
static float pillar_accel;

float random_piller_y(void);
void init_game(void);
void update_game(void);
void draw_game(void);

#endif // FLAPPY_BIRD_H
