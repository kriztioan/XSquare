/**
 *  @file   Levels.cpp
 *  @brief  Levels Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-26
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Levels.h"

Levels::Levels() {

  Levels::map = NULL;
  Levels::rows = 0;
  Levels::cols = 0;

  Levels::backgroundImage = NULL;
  Levels::tilesImage = NULL;
  Levels::spriteIDs = NULL;
  Levels::playing = NULL;
  Levels::players = NULL;
}

Levels::~Levels() {

  Player *player;
  while (Levels::players != NULL) {
    Levels::engine->interface->freeVideoImage(Levels::players->playerImage);

    // now using env....
    // if(Levels::players->name != NULL)
    //  delete Levels::players->name;

    player = Levels::players->previous;
    free(Levels::players);

    Levels::players = player;
  }

  Sprite *spriteID;
  while (Levels::spriteIDs != NULL) {

    Levels::engine->interface->freeVideoImage(Levels::spriteIDs->spriteImage);

    spriteID = Levels::spriteIDs->previous;
    free(Levels::spriteIDs);

    Levels::spriteIDs = spriteID;
  }

  Playing *play;
  while (Levels::playing != NULL) {

    play = Levels::playing->previous;
    free(Levels::playing);

    Levels::playing = play;
  }

  if (Levels::map != NULL)
    delete Levels::map;

  if (Levels::tilesImage != NULL)
    Levels::engine->interface->freeVideoImage(Levels::tilesImage);

  if (Levels::backgroundImage != NULL)
    Levels::engine->interface->freeVideoImage(Levels::backgroundImage);
}

int Levels::createLevelFromFile(const char *filename) {

  if (Levels::engine->interface->messagesOn)
    fprintf(Levels::engine->interface->messageFile,
            "    Levels......loading level from file \'%s\'\n", filename);

  FILE *fp = fopen(filename, "r");
  if (!fp) {
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......cannot open file \'%s\'\n", filename);
    return (1);
  }

  char header[8];
  size_t char_read = fread(header, sizeof(char), 7, fp) / sizeof(char);
  header[char_read] = '\0';
  if (strcmp(header, LEV_SIG) != 0) {
    fclose(fp);
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......failed to create level, file is not %s\n",
              LEV_SIG);
    return (1);
  }

  if (fscanf(fp, " %u %u", &(Levels::cols), &(Levels::rows)) < 2) {
    fclose(fp);
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......unrecognized format\n");
    return (1);
  }

  if (Levels::engine->interface->messagesOn)
    fprintf(Levels::engine->interface->messageFile,
            "    Levels......level dimensions %d x %d\n", Levels::cols,
            Levels::rows);

  char str[256];

  if (!fscanf(fp, " %255s", str)) {
    fclose(fp);
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......unrecognized format\n");
    return (1);
  }

  if (strcmp(str, LEV_BACKGROUND) != 0) {
    fclose(fp);
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......unrecognized format\n");
    return (1);
  }

  if (!fscanf(fp, " %255s", str)) {
    fclose(fp);
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......unrecognized format\n");
    return (1);
  }

  Levels::backgroundImage =
      Levels::engine->interface->createVideoImageFromFile(str);
  Levels::engine->interface->setVideoBackgroundImage(Levels::backgroundImage);

  if (!fscanf(fp, " %255s", str)) {
    fclose(fp);
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......unrecognized format\n");
    return (1);
  }

  if (strcmp(str, LEV_TILES) != 0) {
    fclose(fp);
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......unrecognized format\n");
    return (1);
  }

  if (!fscanf(fp, " %255s", str)) {
    fclose(fp);
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......unrecognized format\n");
    return (1);
  }

  if (Levels::engine->interface->messagesOn)
    fprintf(Levels::engine->interface->messageFile,
            "    Levels......loading tiles from file \'%s\'\n", str);

  if (Levels::tilesImage != NULL)
    Levels::engine->interface->freeVideoImage(Levels::tilesImage);

  Levels::tilesImage = Levels::engine->interface->createVideoImageFromFile(str);
  if (fscanf(fp, " %u %u", &(Levels::width), &(Levels::height)) < 2) {
    fclose(fp);
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......unrecognized format\n");
    return (1);
  }

  if (Levels::engine->interface->messagesOn)
    fprintf(Levels::engine->interface->messageFile,
            "    Levels......tile dimensions %d x %d\n", Levels::width,
            Levels::height);

  if (Levels::map != NULL)
    delete Levels::map;
  Levels::map = new int[Levels::cols * Levels::rows];

  int value;
  for (unsigned int y = 0; y < rows; y++) {
    for (unsigned int x = 0; x < cols; x++) {
      if (!fscanf(fp, " %d ", &value)) {
        fclose(fp);
        if (Levels::engine->interface->messagesOn)
          fprintf(Levels::engine->interface->messageFile,
                  "    Levels......unrecognized format\n");
        return (1);
      }
      Levels::map[y + x * Levels::rows] = value;
    }
  }

  if (!fscanf(fp, " %255s", str)) {
    fclose(fp);
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......unrecognized format\n");
    return (1);
  }

  if (strcmp(str, LEV_SPRITES) != 0) {
    fclose(fp);
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......unrecognized format\n");
    return (1);
  }

  if (!fscanf(fp, " %u", &value)) {
    fclose(fp);
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......unrecognized format\n");
    return (1);
  }

  Sprite **spritenr = NULL;
  for (int i = 0; i < value; i++) {

    if (!fscanf(fp, " %255s", str)) {
      fclose(fp);
      if (Levels::engine->interface->messagesOn)
        fprintf(Levels::engine->interface->messageFile,
                "    Levels......unrecognized format\n");
      return (1);
    }

    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......loading sprites from file \'%s\'\n", str);

    Sprite *spriteID = new Sprite;

    spriteID->spriteImage =
        Levels::engine->interface->createVideoImageFromFile(str);

    if (fscanf(fp, " %u %u %u", &(spriteID->width), &(spriteID->height),
               &(spriteID->speed)) < 3) {
      fclose(fp);
      if (Levels::engine->interface->messagesOn)
        fprintf(Levels::engine->interface->messageFile,
                "    Levels......unrecognized format\n");
      return (1);
    }
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......sprite dimensions %d x %d, speed %d\n",
              spriteID->width, spriteID->height, spriteID->speed);

    spriteID->previous = Levels::spriteIDs;

    if (spriteID->previous != NULL)
      spriteID->previous->next = spriteID;

    spriteID->next = NULL;

    spritenr = (Sprite **)realloc(spritenr, (i + 1) * sizeof(Sprite *));

    Levels::spriteIDs = spritenr[i] = spriteID;
  }
  if (Levels::engine->interface->messagesOn) {
    if (value == 0 || value > 1)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......%d sprites loaded\n", value);
    else
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......%d sprite loaded\n", value);
  }

  int *spritemap = new int[Levels::cols * Levels::rows];

  for (unsigned int y = 0; y < rows; y++) {
    for (unsigned int x = 0; x < cols; x++) {
      if (!fscanf(fp, " %d ", &value)) {
        fclose(fp);
        if (Levels::engine->interface->messagesOn)
          fprintf(Levels::engine->interface->messageFile,
                  "    Levels......unrecognized format\n");
        return (1);
      }
      spritemap[y + x * Levels::rows] = value;
    }
  }

  for (unsigned int y = 0; y < Levels::rows; y++) {
    for (unsigned int x = 0; x < Levels::cols; x++) {

      value = Levels::map[y + x * Levels::rows];
      if (value > -1)
        Levels::engine->interface->copyVideoImage(
            Levels::tilesImage, Levels::backgroundImage, value * Levels::width,
            0, Levels::width, Levels::height, x * Levels::width,
            y * Levels::height);

      value = spritemap[y + x * Levels::rows];
      if (value > -1) {

        Playing *play = new Playing;

        play->sprite = spritenr[value];
        play->xpos = x * Levels::height;
        play->ypos = y * Levels::width;
        play->frame = play->pulse = play->offset_x = play->offset_y = 0;

        play->previous = Levels::playing;
        if (play->previous != NULL)
          play->previous->next = play;

        play->next = NULL;

        Levels::playing = play;
      }
    }
  }

  delete[] spritemap;
  delete spritenr;

  if (!fscanf(fp, " %255s", str)) {
    fclose(fp);
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......unrecognized format\n");
    return (1);
  }

  if (strcmp(str, LEV_PLAYERS) != 0) {
    fclose(fp);
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......unrecognized format\n");
    return (1);
  }

  if (!fscanf(fp, " %u", &value)) {
    fclose(fp);
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......unrecognized format\n");
    return (1);
  }

  for (int i = 0; i < value; i++) {

    if (!fscanf(fp, " %255s", str)) {
      fclose(fp);
      if (Levels::engine->interface->messagesOn)
        fprintf(Levels::engine->interface->messageFile,
                "    Levels......unrecognized format\n");
      return (1);
    }

    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......loading players from file \'%s\'\n", str);

    Player *player = new Player;

    player->playerImage =
        Levels::engine->interface->createVideoImageFromFile(str);

    player->name = getenv("USER");
    player->score = 0;
    player->flags = PLAYER_ALIVE;

    if (fscanf(fp, " %u %u %u %u %u", &(player->width), &(player->height),
               &(player->speed), &(player->xpos), &(player->ypos)) < 5) {
      fclose(fp);
      if (Levels::engine->interface->messagesOn)
        fprintf(Levels::engine->interface->messageFile,
                "    Levels......unrecognized format\n");
      return (1);
    }
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......player dimensions %d x %d, speed %d, player "
              "starting point at (%d,%d)\n",
              player->width, player->height, player->speed, player->xpos,
              player->ypos);

    player->xpos *= width;
    player->ypos *= height;

    if ((Levels::engine->interface->xwidth - player->width) / 2 <=
        player->xpos) {
      if (Levels::width * Levels::cols >=
          player->xpos +
              (Levels::engine->interface->xwidth - player->width) / 2)
        Levels::engine->interface->xvisible =
            player->xpos -
            (Levels::engine->interface->xwidth - player->width) / 2;
      else
        Levels::engine->interface->xvisible =
            Levels::cols * Levels::width - Levels::engine->interface->xwidth;
    } else
      Levels::engine->interface->xvisible = 0;

    if ((Levels::engine->interface->xheight - player->height) / 2 <=
        player->ypos) {
      if (Levels::height * Levels::rows >=
          player->ypos +
              (Levels::engine->interface->xheight - player->height) / 2)
        Levels::engine->interface->yvisible =
            player->ypos -
            (Levels::engine->interface->xheight - player->height) / 2;
      else
        Levels::engine->interface->yvisible =
            Levels::rows * Levels::height - Levels::engine->interface->xheight;
    } else
      Levels::engine->interface->yvisible = 0;

    player->m_col = ((player->xpos + player->width) / Levels::width) - 1;
    player->m_row = player->ypos / Levels::height;

    player->row = player->frame = player->pulse = 0;

    player->action = 0x0;

    player->previous = Levels::players;
    if (player->previous != NULL)
      player->previous->next = player;
    player->next = NULL;

    Levels::players = player;
  }
  if (Levels::engine->interface->messagesOn) {
    if (value == 0 || value > 1)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......%d players loaded\n", value);
    else
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......%d player loaded\n", value);
  }

  if (!fscanf(fp, " %255s", str)) {
    fclose(fp);
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......unrecognized format\n");
    return (1);
  }

  if (strcmp(str, LEV_SOUNDS) != 0) {
    fclose(fp);
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......unrecognized format\n");
    return (1);
  }

  if (!fscanf(fp, " %u", &value)) {
    fclose(fp);
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......unrecognized format\n");
    return (1);
  }

  for (int i = 0; i < value; i++) {

    if (!fscanf(fp, " %255s", str)) {
      fclose(fp);
      if (Levels::engine->interface->messagesOn)
        fprintf(Levels::engine->interface->messageFile,
                "    Levels......unrecognized format\n");
      return (1);
    }

    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......found sound file \'%s\'\n", str);

    Levels::engine->interface->loadSoundFromFile(str);
  }
  if (Levels::engine->interface->messagesOn) {
    if (value == 0 || value > 1)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......loaded %d sound files\n", value);
    else
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......loaded %d sound file\n", value);
  }

  fclose(fp);

  return (0);
}

void Levels::draw() {

  Player *player = Levels::players;
  while (player != NULL) {

    Levels::engine->interface->drawVideoImage(
        player->playerImage, player->width * player->frame,
        player->height * player->row, player->width, player->height,
        player->xpos, player->ypos);

    if ((player->xpos - Levels::engine->interface->xvisible + player->width) >=
            player->width + ((3 * Levels::engine->interface->xwidth) / 4) &&
        (player->xpos <= ((Levels::width * Levels::cols) -
                          (Levels::engine->interface->xwidth / 4))))
      Levels::engine->interface->xvisible =
          player->xpos - (3 * Levels::engine->interface->xwidth / 4);
    else if (((player->xpos - Levels::engine->interface->xvisible) <=
              (Levels::engine->interface->xwidth / 4)) &&
             (player->xpos > Levels::engine->interface->xwidth / 4))
      Levels::engine->interface->xvisible =
          player->xpos - (Levels::engine->interface->xwidth) / 4;

    if ((player->ypos - Levels::engine->interface->yvisible + player->height) >=
            player->height + ((3 * Levels::engine->interface->xheight) / 4 -
                              player->height) &&
        (player->ypos <= ((Levels::height * Levels::rows) - player->height -
                          (Levels::engine->interface->xheight / 4))))
      Levels::engine->interface->yvisible =
          player->ypos - ((3 * Levels::engine->interface->xheight) / 4) +
          2 * Levels::height;
    else if ((player->ypos - Levels::engine->interface->yvisible) <=
                 (Levels::engine->interface->xheight / 4) &&
             (player->ypos >= Levels::engine->interface->xheight / 4))
      Levels::engine->interface->yvisible =
          player->ypos - (Levels::engine->interface->xheight) / 4;

    player = player->previous;
  }

  Playing *play = Levels::playing;
  while (play != NULL) {

    play->pulse++;
    if (play->pulse % play->sprite->speed == 0) {
      play->frame++;

      if (play->frame < ((Levels::engine->interface->getVideoImageWidth(
                              play->sprite->spriteImage) /
                          play->sprite->width) *
                         (Levels::engine->interface->getVideoImageHeight(
                              play->sprite->spriteImage) /
                          play->sprite->height))) {

        play->offset_y =
            play->frame / (Levels::engine->interface->getVideoImageWidth(
                               play->sprite->spriteImage) /
                           play->sprite->width);
        play->offset_x =
            play->frame % (Levels::engine->interface->getVideoImageWidth(
                               play->sprite->spriteImage) /
                           play->sprite->width);
      } else
        play->offset_x = play->offset_y = play->frame = 0;
    }

    if ((play->xpos + Levels::width >= Levels::engine->interface->xvisible) &&
        (play->xpos <= Levels::engine->interface->xvisible +
                           Levels::engine->interface->xwidth + Levels::width))
      Levels::engine->interface->drawVideoImage(
          play->sprite->spriteImage, play->sprite->width * play->offset_x,
          play->sprite->height * play->offset_y, play->sprite->width,
          play->sprite->height, play->xpos, play->ypos);
    play = play->previous;
  }
}

void Levels::freeLevelsPlaying(Playing *play) {

  if (play->next != NULL)
    play->next->previous = play->previous;
  if (play->previous != NULL)
    play->previous->next = play->next;

  if (Levels::playing == play)
    Levels::playing = play->previous;

  free(play);
}

int Levels::createMultiplayerGame() {

  // Playaer *networkplayers = (Player *)
  // Levels::enginge->interface->networkHandshake();

  // while(networkplayers != NULL){

  for (int i = 2; --i;) {

    Player *multiplayer = new Player;

    // Need network name...don't we ;-)
    multiplayer->name = getenv("USER");
    if (Levels::engine->interface->messagesOn)
      fprintf(Levels::engine->interface->messageFile,
              "    Levels......loading multiplayer \'%s\'\n",
              multiplayer->name);

    multiplayer->score = 0;
    multiplayer->flags = PLAYER_ALIVE;

    /* Need player mechanism.....(same as sprites with number....) */
    multiplayer->playerImage =
        Levels::engine->interface->createVideoImageFromFile("data/player.png");
    multiplayer->width = 25;
    multiplayer->height = 34;
    multiplayer->speed = 4;
    multiplayer->xpos = 10;
    multiplayer->ypos = 7;
    /* End of temp */

    multiplayer->xpos *= width;
    multiplayer->ypos *= height;

    multiplayer->m_col =
        ((multiplayer->xpos + multiplayer->width) / Levels::width) - 1;
    multiplayer->m_row = multiplayer->ypos / Levels::height;

    multiplayer->row = multiplayer->frame = multiplayer->pulse = 0;

    multiplayer->action = 0x0;

    multiplayer->previous = Levels::players;
    multiplayer->next = NULL;

    if (multiplayer->previous != NULL)
      multiplayer->previous->next = multiplayer;

    Levels::players = multiplayer;

    // networkplayers = networkplayers->previous;
  }

  return (1);
}
