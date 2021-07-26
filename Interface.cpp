/**
 *  @file   Interface.cpp
 *  @brief  Interface Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-26
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Interface.h"

Interface::Interface() {

  Interface::menuOptions = new char *[6];
  Interface::menuOptions[0] = new char[strlen("New game") + 1];
  strcpy(Interface::menuOptions[0], "New game");
  Interface::menuOptions[1] = new char[strlen("Load game") + 1];
  strcpy(Interface::menuOptions[1], "Load game");
  Interface::menuOptions[2] = new char[strlen("Network") + 1];
  strcpy(Interface::menuOptions[2], "Network");
  Interface::menuOptions[3] = new char[strlen("Credits") + 1];
  strcpy(Interface::menuOptions[3], "Credits");
  Interface::menuOptions[4] = new char[strlen("Exit game") + 1];
  strcpy(Interface::menuOptions[4], "Exit game");
  Interface::menuOptions[5] = NULL;

  Interface::engine = NULL;

  Interface::backgroundImage = NULL;
  Interface::selectorImage = NULL;

  Interface::messagesOn = true;
  Interface::messageFile = stdout;

  Interface::interfaceOn = true;

  Video::interface = Audio::interface = Network::interface = this;
}

Interface::~Interface() {

  char **option = Interface::menuOptions;

  while (*option != NULL) {
    delete *option;
    option++;
  }
  delete Interface::menuOptions;

  if (Interface::backgroundImage != NULL)
    Video::freeVideoImage(Interface::backgroundImage);

  if (Interface::selectorImage != NULL)
    Video::freeVideoImage(Interface::selectorImage);

  if (Interface::engine != NULL) {
    delete Interface::engine;
  }

  Video::interface->videoStop();
  Audio::interface->audioStop();
  Network::interface->networkStop();

  if (Interface::messagesOn) {
    fprintf(Interface::messageFile, "Interface.......stopped\n");
    fprintf(Interface::messageFile, "Logging.........stopped\n");
  }

  fclose(Interface::messageFile);
}

void Interface::interfaceStart(int argc, char *argv[], char **envp) {

  if (Interface::messagesOn)
    fprintf(Interface::messageFile, "Interface.......starting\n");

  const char *exe = getenv("_");
  if (exe) {

    char *path = dirname((char *)exe);
    if (path && *path != '.') {

      if (!chdir(path))
        fprintf(Interface::messageFile,
                "Interface.......changed directory to: %s\n", path);
      else
        fprintf(Interface::messageFile,
                "Interface.......failed to change directory to: %s\n", path);
    }
  }

  if (Video::videoStart(argc, argv, envp)) {
    fprintf(Interface::messageFile,
            "Interface.......failed to initialize video\n");
    abort();
  }

  Audio::audioStart();
  Network::networkStart();

  Interface::backgroundImage =
      Video::createVideoImageFromFile(INTERFACE_BACKGROUND);
  Interface::selectorImage =
      Video::createVideoImageFromFile(INTERFACE_SELECTOR);

  if (Interface::backgroundImage != NULL && Interface::selectorImage != NULL)
    Video::setVideoBackgroundImage(Interface::backgroundImage);

  int *keysPressed = NULL, *act = NULL;

  fd_set readfds, testfds;
  FD_ZERO(&readfds);
  FD_SET(Video::videoFD, &readfds);
  testfds = readfds;

  int selector = 0, n_options = 0;

  while (Interface::menuOptions[n_options] != NULL)
    n_options++;

  while (Interface::interfaceOn) {

    Interface::drawMenu(selector);

    Video::videoSync();

    if (select(Video::videoFD + 1, &testfds, NULL, NULL, NULL) == -1) {
      Interface::interfaceOn = false;
      break;
    }

    if (FD_ISSET(Video::videoFD, &testfds)) {

      if (Video::getVideoInput(&keysPressed))
        Interface::interfaceOn = false;

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

          if (Interface::menuOptions[selector + 1] != NULL)
            selector++;
          else
            selector = 0;

          break;

        case ACT_RETURN:

          switch (selector) {

          case 0:

            engine = new Engine();
            Interface::engine->interface = this;
            Interface::engine->engineStart("data/level1.lev");
            delete engine;
            engine = NULL;

            if (Interface::backgroundImage != NULL &&
                Interface::selectorImage != NULL)
              Video::setVideoBackgroundImage(Interface::backgroundImage);

            break;

          case 1:

            Interface::drawLoadGame();

            break;

          case 2:

            if (Network::interface->networkOn) {

              engine = new Engine();
              Interface::engine->interface = this;
              Interface::engine->multiplayer = true;
              Interface::engine->engineStart("data/level0.lev");
              delete engine;
              engine = NULL;

              if (Interface::backgroundImage != NULL &&
                  Interface::selectorImage != NULL)
                Video::setVideoBackgroundImage(Interface::backgroundImage);
            }

            break;

          case 3:

            Interface::drawCredits();

            break;

          default:

            Interface::interfaceOn = false;

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
  }
}

void Interface::drawMenu(int selected) {

  char **opt = Interface::menuOptions;

  int offset = 60;

  if (Interface::selectorImage != NULL)
    Video::drawVideoImage(Interface::selectorImage, 0, 0,
                          Video::getVideoImageWidth(Interface::selectorImage),
                          Video::getVideoImageHeight(Interface::selectorImage),
                          40, selected * 25 + offset - 11);

  while (*opt != NULL) {
    Video::drawVideoString(strlen(*opt), *opt, 50, offset);
    offset += 25;
    opt++;
  }
}

void Interface::drawCredits() {

  Video::drawVideoString(strlen("Credits"), "Credits", 150, 30);
  Video::drawVideoString(strlen("Game design and"), "Game design and ", 150,
                         60);
  Video::drawVideoString(strlen("Game implementation"), "Game implementation",
                         150, 80);
  Video::drawVideoString(strlen("by Christiaan Boersma"),
                         "by Christiaan Boersma", 150, 100);
  Video::drawVideoString(strlen("Press any key"), "Press any key", 150, 150);
}

void Interface::drawLoadGame() {

  Video::drawVideoString(strlen("Load game"), "Load game", 150, 84);
}
