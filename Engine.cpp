/**
 *  @file   Engine.cpp
 *  @brief  Engine Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-26
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Engine.h"

Engine::Engine() {

  Engine::menuOptions = new char *[4];
  Engine::menuOptions[0] = new char[strlen("Save game") + 1];
  strcpy(Engine::menuOptions[0], "Save game");

  Engine::menuOptions[1] = new char[strlen("Load game") + 1];
  strcpy(Engine::menuOptions[1], "Load game");

  Engine::menuOptions[2] = new char[strlen("Return") + 1];
  strcpy(Engine::menuOptions[2], "Return");

  Engine::menuOptions[3] = NULL;

  Engine::keysPressed = NULL;
  Engine::keysDown = NULL;

  Engine::interface = NULL;
  Engine::engineOn = false;
  Engine::multiplayer = false;

  Engine::barImage = NULL;
  Engine::dbImage = NULL;
  Engine::soundImage = NULL;
  Engine::menuImage = NULL;
}

Engine::~Engine() {

  char **option = Engine::menuOptions;

  while (*option != NULL) {
    delete *option;
    option++;
  }

  if (Engine::menuOptions != NULL)
    delete Engine::menuOptions;

  if (Engine::barImage != NULL)
    Engine::interface->freeVideoImage(Engine::barImage);

  if (Engine::dbImage != NULL)
    Engine::interface->freeVideoImage(Engine::dbImage);

  if (Engine::soundImage != NULL)
    Engine::interface->freeVideoImage(Engine::soundImage);

  if (Engine::menuImage != NULL)
    Engine::interface->freeVideoImage(Engine::menuImage);

  if (Engine::keysPressed != NULL)
    delete Engine::keysPressed;

  if (Engine::keysDown != NULL)
    delete Engine::keysDown;
}

void Engine::engineStart(const char *filename) {

  Engine::engineOn = true;
  if (Engine::interface->messagesOn)
    fprintf(Engine::interface->messageFile, "Engine..........starting\n");

  Engine::barImage = Engine::interface->createVideoImageFromFile(INFO_BAR);
  Engine::dbImage = Engine::interface->createVideoImageFromFile(INFO_DB);
  Engine::soundImage = Engine::interface->createVideoImageFromFile(INFO_SOUND);

  Engine::menuImage = Engine::interface->createVideoImageFromFile(MENU_BACK);

  Levels::engine = this;

  if (Engine::createLevelFromFile(filename) == 0) {

    if (Engine::multiplayer) {

      if (Engine::createMultiplayerGame() == 0)
        Engine::engineOn = false;
    }

    Engine::interface->playSound(0, 1);

    while (Engine::engineOn) {

      Engine::actOnPlayers();

      Levels::draw();

      Engine::drawInfo();

      if (Engine::multiplayer)
        Engine::interface->networkSync();

      Engine::interface->videoSync();
    }

    Levels::engine->interface->xvisible = Levels::engine->interface->yvisible =
        0;

    Engine::interface->audioClear();
  }

  if (Engine::interface->messagesOn)
    fprintf(Engine::interface->messageFile, "Engine..........stopped\n");
}

void Engine::actOnPlayers() {

  // what if we have more players ???

  Player *player = Levels::players;

  int *act = NULL;

  if (++player->pulse % player->speed == 0) {

    player->m_col = ((player->xpos + (player->width / 2)) / Levels::width);
    player->m_row = player->ypos / Levels::height;

    if ((player->ypos % 25) != 0)
      player->ypos += 25 - (player->ypos % 25);
  }

  delete Engine::keysPressed;

  Engine::keysPressed = NULL;

  if (Engine::interface->getVideoInput(&(Engine::keysPressed),
                                       &(Engine::keysDown)))
    Engine::engineOn = false;

  act = Engine::keysPressed;

  if (act != NULL) {
    while (*act != ACT_END) {

      switch (*act) {

      case ACT_MENU:

        Engine::menu();
        break;

      case ACT_DB:

        Engine::interface->toggleVideoDB();
        break;

      case ACT_SOUND:

        Engine::interface->toggleAudioMute();
        break;

      default:
        break;
      }
      act++;
    }
  }

  int action;

  if (player->flags == PLAYER_DEAD)
    action = DIE;
  else if (actionWanted(Engine::keysPressed, Engine::keysDown, ACT_RIGHT)) {

    if (actionWanted(Engine::keysPressed, Engine::keysDown, ACT_UP))
      action = JUMP_RIGHT;
    else if (actionWanted(Engine::keysPressed, Engine::keysDown, ACT_SHIFT_R) ||
             actionWanted(Engine::keysPressed, Engine::keysDown, ACT_SHIFT_L))
      action = RUN_RIGHT;
    else if (actionWanted(Engine::keysPressed, Engine::keysDown, ACT_ALT_R) ||
             actionWanted(Engine::keysPressed, Engine::keysDown, ACT_ALT_L))
      action = PICKUP_RIGHT;
    else
      action = WALK_RIGHT;
  } else if (actionWanted(Engine::keysPressed, Engine::keysDown, ACT_LEFT)) {

    if (actionWanted(Engine::keysPressed, Engine::keysDown, ACT_UP))
      action = JUMP_LEFT;
    else if (actionWanted(Engine::keysPressed, Engine::keysDown, ACT_SHIFT_R) ||
             actionWanted(Engine::keysPressed, Engine::keysDown, ACT_SHIFT_L))
      action = RUN_LEFT;
    else if (actionWanted(Engine::keysPressed, Engine::keysDown, ACT_ALT_R) ||
             actionWanted(Engine::keysPressed, Engine::keysDown, ACT_ALT_L))
      action = PICKUP_LEFT;
    else
      action = WALK_LEFT;
  } else if (actionWanted(Engine::keysPressed, Engine::keysDown, ACT_UP) &&
             (Levels::map[player->m_row + 1 + player->m_col * Levels::rows] !=
              -1))
    action = JUMP_UP;
  else if (actionWanted(Engine::keysPressed, Engine::keysDown, ACT_SPACE))
    action = ACTION;
  else if (player->frame >= 4)
    action = STAND;
  else
    action = player->action;

  if (action != ACT_UP &&
      Levels::map[player->m_row + 1 + player->m_col * Levels::rows] == -1)
    action = FALL;

  if (action != player->action) {
    player->action = action;
    player->frame = 0;
  }

  if (action != STAND) {

    if (player->frame >= 4)
      player->frame = 1;
    else
      player->frame++;

    switch (player->action) {

    case WALK_RIGHT:
      player->row = 0;
      player->xpos += 5;
      break;

    case RUN_RIGHT:
      player->row = 4;
      player->xpos += 10;
      break;

    case JUMP_RIGHT:
      player->row = 2;
      player->xpos += 10;
      player->ypos -= 12;
      break;

    case PICKUP_RIGHT:
      player->row = 0;
      break;

    case WALK_LEFT:
      player->row = 1;
      player->xpos -= 5;
      break;

    case RUN_LEFT:
      player->row = 5;
      player->xpos -= 10;
      break;

    case JUMP_LEFT:
      player->row = 3;
      player->xpos -= 10;
      player->ypos -= 12;
      break;

    case PICKUP_LEFT:
      player->row = 1;
      break;

    case JUMP_UP:
      player->frame = 0;
      player->ypos -= 10;
      break;

    case FALL:

      player->frame = 0;

      player->ypos += 12;

      player->m_col = ((player->xpos + (player->width / 2)) / Levels::width);
      player->m_row = player->ypos / Levels::height;

      if (Levels::map[player->m_row + 1 + player->m_col * Levels::rows] != -1)
        player->ypos -= 12;

      break;

    case ACTION:
      Engine::interface->playSound(2);
      player->row = 6;
      break;

    case DIE:

      break;
    }
  } else
    player->frame = 0;

  if (player->xpos + player->width <= player->width)
    player->xpos = 0;
  else if (player->xpos + player->width > Levels::cols * Levels::width)
    player->xpos = Levels::cols * Levels::width - player->width;

  if (player->ypos + player->height <= player->height)
    player->ypos = 0;
  else if (player->ypos + player->height > Levels::rows * Levels::height)
    player->ypos = Levels::rows * Levels::height - player->height;

  Playing *play = Levels::playing, *tmp;
  while (play != NULL) {

    if ((play->xpos - player->xpos) * (play->xpos - player->xpos) +
            (play->ypos - player->ypos) * (play->ypos - player->ypos) <
        150) {
      player->score++;
      Engine::interface->playSound(1);

      tmp = play->previous;

      Levels::freeLevelsPlaying(play);

      play = tmp;
    } else
      play = play->previous;
  }
}

void Engine::drawInfo() {

  int width = Engine::interface->getVideoImageWidth(Engine::barImage),
      height = Engine::interface->getVideoImageHeight(Engine::barImage);

  Engine::interface->drawVideoImage(Engine::barImage, 0, 0, width, height,
                                    Engine::interface->xvisible,
                                    Engine::interface->yvisible);

  width = Engine::interface->getVideoImageWidth(Engine::dbImage);
  height = Engine::interface->getVideoImageHeight(Engine::dbImage);

  if (Engine::interface->xdbe)
    Engine::interface->drawVideoImage(Engine::dbImage, 0, 0, width, height,
                                      Engine::interface->xvisible +
                                          Engine::interface->xwidth - width - 1,
                                      Engine::interface->yvisible + 1);

  int offset = width + 4;

  width = Engine::interface->getVideoImageWidth(Engine::soundImage);
  height = Engine::interface->getVideoImageHeight(Engine::soundImage);

  if (!(Engine::interface->mute))
    Engine::interface->drawVideoImage(Engine::soundImage, 0, 0, width, height,
                                      Engine::interface->xvisible +
                                          Engine::interface->xwidth - width -
                                          offset - 1,
                                      Engine::interface->yvisible + 1);

  size_t i, score_len;

  char *score = NULL;

  Player *player = Levels::players;
  while (player != NULL) {

    score_len = strlen(player->name) + 2;
    i = player->score;
    while (i /= 10)
      score_len++;
    score = new char[score_len + 1];
    snprintf(score, score_len + 1, "%s %u", player->name, player->score);
    Levels::engine->interface->drawVideoString(
        score_len, score, Levels::engine->interface->xvisible + 10,
        Levels::engine->interface->yvisible + 11);

    delete score;
    player = player->previous;
  }
}

void Engine::menu() {

  int *keysPressed = NULL, *act = NULL;

  fd_set readfds, testfds;
  FD_ZERO(&readfds);
  FD_SET(Engine::interface->videoFD, &readfds);
  testfds = readfds;

  int selector = 0, n_options = 0;

  while (Engine::menuOptions[n_options] != NULL)
    n_options++;

  bool menuOn = true;

  int width = Engine::interface->getVideoImageWidth(Engine::menuImage),
      height = Engine::interface->getVideoImageHeight(Engine::menuImage);

  while (menuOn) {

    Engine::interface->drawVideoImage(Engine::menuImage, 0, 0, width, height,
                                      Engine::interface->xvisible,
                                      Engine::interface->yvisible);
    Engine::drawMenu(selector);
    Engine::interface->videoSync();

    if (select(Engine::interface->videoFD + 1, &testfds, NULL, NULL, NULL) ==
        -1) {
      menuOn = false;
      break;
    }

    if (FD_ISSET(Engine::interface->videoFD, &testfds)) {

      if (Engine::interface->getVideoInput(&keysPressed))
        menuOn = false;

      act = keysPressed;
      while (*act != ACT_END) {

        switch (*act++) {

        case ACT_UP:

          if (selector > 0)
            selector--;
          else
            selector = n_options - 1;

          break;

        case ACT_DOWN:

          if (Engine::menuOptions[selector + 1] != NULL)
            selector++;
          else
            selector = 0;

          break;

        case ACT_RETURN:

          switch (selector) {

          case 0:
            break;

          case 1:
            break;

          default:

            menuOn = false;

            break;
          };

          break;

        default:

          break;
        }
      }
      delete keysPressed;
      keysPressed = NULL;
    }
    testfds = readfds;
  }
}

void Engine::drawMenu(int selected) {

  char **opt = Engine::menuOptions;

  int offset = 70;

  if (Engine::interface->selectorImage != NULL)
    Engine::interface->drawVideoImage(
        Engine::interface->selectorImage, 0, 0,
        Engine::interface->getVideoImageWidth(Engine::interface->selectorImage),
        Engine::interface->getVideoImageHeight(
            Engine::interface->selectorImage),
        Engine::interface->xvisible + 40,
        Engine::interface->yvisible + selected * 25 + offset - 11);

  while (*opt != NULL) {
    Engine::interface->drawVideoString(strlen(*opt), *opt,
                                       Engine::interface->xvisible + 50,
                                       Engine::interface->yvisible + offset);
    offset += 25;
    opt++;
  }
}

bool Engine::actionWanted(int *down, int *pressed, int key) {

  if (pressed != NULL) {
    while (*pressed != ACT_END) {

      if (*pressed == key)
        return (true);

      pressed++;
    }
  }

  if (down != NULL) {
    while (*down != ACT_END) {

      if (*down == key)
        return (true);

      down++;
    }
  }

  return (false);
}
