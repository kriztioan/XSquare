/**
 *  @file   Interface.h
 *  @brief  Interface Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-26
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "Audio.h"
#include "Network.h"
#include "Video.h"

#include <cstdio>
#include <cstdlib>
#include <libgen.h>

#define INTERFACE_BACKGROUND "data/interface.png"
#define INTERFACE_SELECTOR "data/selector.png"

class Engine;

class Interface : public Video, public Audio, public Network {

  friend class Video;
  friend class Audio;
  friend class Network;

  friend class Engine;
  friend class Levels;
  friend class Players;
  friend class Sprites;

public:
  Interface();
  ~Interface();

  void interfaceStart(int argc, char *argv[], char **envp);
  void setInterfaceMessageFile(const char *filename);
  void setInterfaceMessagesOn(bool option = true);

protected:
  Engine *engine;

  bool messagesOn;

  FILE *messageFile;

private:
  void drawMenu(int selected = 0);

  void drawCredits();
  void drawLoadGame();

  char **menuOptions;

  void *backgroundImage;
  void *selectorImage;

  bool interfaceOn;
};

#include "Engine.h"

inline void Interface::setInterfaceMessagesOn(bool option) {

  Interface::messagesOn = option;
  if (Interface::messagesOn)
    fprintf(Interface::messageFile, "Logging.........starting\n");
}

inline void Interface::setInterfaceMessageFile(const char *filename) {

  Interface::messageFile = fopen(filename, "w");
  if (Interface::messagesOn)
    fprintf(Interface::messageFile,
            "Logging.........re-directing to file \'%s\' \n", filename);
}

#endif // End of INTERFACE_H_
