/**
 *  @file   Engine.h
 *  @brief  Engine Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-26
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef ENGINE_H_
#define ENGINE_H_

#include "Levels.h"
#include <time.h>

/* X11 KeyDefs */

#define ACT_LEFT 0xFF51
#define ACT_UP 0xFF52
#define ACT_RIGHT 0xFF53
#define ACT_DOWN 0xFF54

#define ACT_RETURN 0xFF0D
#define ACT_SPACE 0x020

#define ACT_SHIFT_L 0xFFE1
#define ACT_SHIFT_R 0xFFE2
#define ACT_ALT_L 0xFFE9
#define ACT_ALT_R 0xFFEA

#define ACT_MENU 0x06d

#define ACT_DB 0x064
#define ACT_SOUND 0x073

#define ACT_END 0xFFFFFF

#define SOUND_CONT -1

#define INFO_BAR "data/bar.png"
#define INFO_DB "data/db.png"
#define INFO_SOUND "data/sound.png"

#define MENU_BACK "data/menu.png"

#define STAND 0x00
#define WALK_RIGHT 0x01
#define WALK_LEFT 0x02
#define JUMP_RIGHT 0x03
#define JUMP_LEFT 0x04
#define JUMP_UP 0x05
#define RUN_RIGHT 0x06
#define RUN_LEFT 0x07
#define PICKUP_RIGHT 0x08
#define PICKUP_LEFT 0x09
#define FALL 0x0a
#define DIE 0x0b
#define ACTION 0x0c

class Interface;

class Engine : public Levels {

  friend class Levels;

  friend class Interface;

public:
  Engine();
  ~Engine();

  void engineStart(const char *filename);

  void actOnPlayers();

protected:
  Interface *interface;

  bool multiplayer;

private:
  void drawInfo();

  void menu();
  void drawMenu(int selected = 0);

  bool actionWanted(int *down, int *pressed, int key);

  bool engineOn;

  int *keysPressed;
  int *keysDown;

  void *barImage;
  void *dbImage;
  void *soundImage;

  void *menuImage;

  char **menuOptions;
};

#include "Interface.h"

#endif // End of ENGINE_H_
