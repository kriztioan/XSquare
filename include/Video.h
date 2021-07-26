/**
 *  @file   Video.h
 *  @brief  Video Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-26
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef VIDEO_H_
#define VIDEO_H_

#define TEXT_COLOR "#f8f8ff"
#define int_p_NULL (int *)NULL

class Interface;

#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xdbe.h>
#include <X11/keysymdef.h>

#include <cstdio>
#include <cstdlib>
#include <string.h>

#include <png.h>

typedef struct Image {

  Image *next, *previous;

  unsigned int width;
  unsigned int height;

  Pixmap pixmap, maskpixmap;
} Image;

class Video {

public:
  Video();
  ~Video();

  int videoStart(int argc, char *argv[], char **envp);
  void videoStop();

  void setVideoIcon(const char *filename);
  void setVideoWindowTitle(const char *title);
  void setVideoIconTitle(const char *title);
  void setVideoApplicationClass(const char *aclass);
  void setVideoApplicationName(const char *name);
  void setVideoResolution(int width, int height);
  void setVideoFramerate(unsigned int rate);

  void toggleVideoDB();

  void *createVideoImageFromFile(const char *filename);
  void freeVideoImage(void *imageID);

  unsigned int getVideoImageWidth(void *imageID);
  unsigned int getVideoImageHeight(void *imageID);

  void copyVideoImage(void *imageIDsrc, void *imageIDdest, int src_x, int src_y,
                      int width, int height, int dest_x, int dst_y);
  void drawVideoImage(void *imageID, int src_x, int src_y, int width,
                      int height, int dest_x, int dest_y);
  void setVideoBackgroundImage(void *imageID);
  void drawVideoString(size_t len, char *str, int dest_x, int dest_y);
  int getVideoInput(int **keysPressed, int **keysDown = NULL);
  void videoSync();

protected:
  Interface *interface;

  struct timespec sleep;

  int videoFD;

  unsigned int xvisible;
  unsigned int yvisible;

  unsigned int xwidth;
  unsigned int xheight;

  int xpos;
  int ypos;

  bool xdbe;

private:
  int xborderwidth;

  char *xicon;

  char *xwindowtitle;
  char *xicontitle;

  char *xapplicationclass;
  char *xapplicationname;

  bool videoOn;

  Display *xdisplay;
  Window xwindow;

  XdbeBackBuffer xdbebackbuffer;
  XdbeSwapInfo xdbeswapinfo;

  XEvent xevent;

  Image *imageIDs;

  Pixmap backgroundpixmap;
  Pixmap drawpixmap;
  Pixmap offscreenpixmap;

  Pixmap maskpixmap;
  Pixmap tmpmaskpixmap;

  GC maskGC;
  GC unmaskGC;

  GC drawGC;

  GC copyGC;

  bool empty;
};

#include "Interface.h"

inline void Video::setVideoIcon(const char *filename) {

  if (Video::xicon != NULL)
    delete Video::xicon;

  Video::xicon = new char[strlen(filename) + 1];
  strcpy(Video::xicon, filename);
}

inline void Video::setVideoWindowTitle(const char *title) {

  if (Video::xwindowtitle != NULL)
    delete Video::xwindowtitle;

  Video::xwindowtitle = new char[strlen(title) + 1];
  strcpy(Video::xwindowtitle, title);
}

inline void Video::setVideoIconTitle(const char *title) {

  if (Video::xicontitle != NULL)
    delete Video::xicontitle;

  Video::xicontitle = new char[strlen(title) + 1];
  strcpy(Video::xicontitle, title);
}

inline void Video::setVideoApplicationClass(const char *aclass) {

  if (Video::xapplicationclass != NULL)
    delete Video::xapplicationclass;

  Video::xapplicationclass = new char[strlen(aclass) + 1];
  strcpy(Video::xapplicationclass, aclass);
}

inline void Video::setVideoApplicationName(const char *name) {

  if (Video::xapplicationname != NULL)
    delete Video::xapplicationname;

  Video::xapplicationname = new char[strlen(name) + 1];
  strcpy(Video::xapplicationname, name);
}

inline void Video::setVideoResolution(int width, int height) {

  Video::xwidth = width;
  Video::xheight = height;
}

inline void Video::setVideoFramerate(unsigned int rate) {

  Video::sleep.tv_sec = 1 / rate;
  Video::sleep.tv_nsec = (1000000000 / rate) % 1000000000;
}

inline unsigned int Video::getVideoImageWidth(void *imageID) {

  Image *ID = (Image *)imageID;
  return (ID->width);
}

inline unsigned int Video::getVideoImageHeight(void *imageID) {
  Image *ID = (Image *)imageID;
  return (ID->height);
}

inline void Video::toggleVideoDB() { Video::xdbe = !(Video::xdbe); }

#endif // End of VIDEO_H_
