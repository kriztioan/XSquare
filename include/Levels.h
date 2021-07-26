/**
 *  @file   Levels.h
 *  @brief  Levels Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-26
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef __Levels__H
#define __Levels__H

#define LEV_SIG "@LEV2K5"
#define LEV_TILES "tiles:"
#define LEV_SPRITES "sprites:"
#define LEV_PLAYERS "players:"
#define LEV_BACKGROUND "background:"
#define LEV_SOUNDS "sounds:"

#define PLAYER_ALIVE 0x000
#define PLAYER_DEAD 0x001

typedef struct Sprite {

  Sprite *next, *previous;

  void *spriteImage;

  unsigned int width;
  unsigned int height;
  unsigned int speed;

} Sprite;

typedef struct Playing {

  Playing *next, *previous;

  Sprite *sprite;

  unsigned int xpos;
  unsigned int ypos;
  unsigned int frame;
  unsigned int pulse;
  unsigned int offset_x;
  unsigned int offset_y;

} Playing;

typedef struct Player {

  Player *next, *previous;

  void *playerImage;

  char *name;
  unsigned int score;

  unsigned int xpos;
  unsigned int ypos;
  unsigned int frame;
  unsigned int row;

  unsigned int m_row;
  unsigned int m_col;

  unsigned int width;
  unsigned int height;

  unsigned int speed;
  unsigned int pulse;

  unsigned int flags;
  int action;

} Player;

class Engine;

class Levels {

  friend class Engine;

public:
  Levels();
  ~Levels();

  int createLevelFromFile(const char *filename);
  int createMultiplayerGame();

  void draw();

protected:
  void freeLevelsPlaying(Playing *play);

  void *backgroundImage;
  void *tilesImage;

  unsigned int cols;
  unsigned int rows;

  int *map;

  unsigned int width;
  unsigned int height;

  Engine *engine;

private:
  Sprite *spriteIDs;

  Playing *playing;

  Player *players;
};

#include "Engine.h"

#endif
