/**
 *  @file   main.cpp
 *  @brief  XSquare
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-26
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "game.h"

int main(int argc, char *argv[], char **envp) {

  printf("\001\033[%d;%d;%dm\002", 0, 97, 41);
  printf(
      "\n==================================================================\n");
  printf("%s version %s by %s %s\n", GAME_TITLE, GAME_VERSION, GAME_AUTHORS,
         GAME_YEAR);
  printf("==================================================================");
  printf("\n\001\033[%d;%d;%dm\002\n", 0, 93, 43);

  Interface interface;
  //  interface.setInterfaceMessageFile("log");
  interface.setInterfaceMessagesOn(true);
  interface.setVideoResolution(300, 200);
  interface.setVideoFramerate(24);
  interface.setVideoIcon(GAME_ICON);
  interface.setVideoWindowTitle(GAME_TITLE);
  interface.setVideoIconTitle(GAME_ICONIZED);
  interface.setVideoApplicationClass(GAME_TITLE);
  interface.setVideoApplicationName(GAME_TITLE);
  interface.setAudioChannels(16);
  interface.setAudioSamplerate(11025);
  interface.setAudioStereo(1);
  interface.setAudioGain(2);
  interface.setNetworkIp("127.0.0.1");
  interface.setNetworkPort(7777);
  interface.interfaceStart(argc, argv, envp);

  printf("\001\033[%d;%d;%dm\002", 0, 97, 41);
  printf(
      "\n==================================================================\n");
  printf("\nGame..........finished\n\n       Thank you for playing %s\n",
         GAME_TITLE);
  printf("==================================================================");
  printf("\n\001\033[%d;%d;%dm\002\n", 0, 93, 43);

  return (0);
}
