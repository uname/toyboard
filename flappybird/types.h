#ifndef __TYPES_H__
#define __TYPES_H__

typedef enum GameState {
  STARTED,
  STOPPED
};

typedef struct Wall {
  byte bricks;
  byte xpos;
};

typedef enum Direction {
  DOWN = -1, 
  STRAIGHT,
  UP
};

typedef struct Game {
  GameState state;

  int score;

  // y velocity
  float vy;

  // y position betw 0 and 1
  float birdY;

  // the walls that slide in from right
  Wall wallOne;
  Wall wallTwo;

  byte framebuffer[8];
};

#endif