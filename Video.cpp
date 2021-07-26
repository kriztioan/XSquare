/**
 *  @file   Video.cpp
 *  @brief  Video Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-26
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Video.h"

Video::Video() {

  Video::sleep.tv_sec = 1;
  Video::sleep.tv_nsec = 0;

  Video::videoFD = -1;

  Video::imageIDs = NULL;

  Video::xicon = NULL;

  Video::xwindowtitle = NULL;
  Video::xicontitle = NULL;

  Video::xapplicationclass = NULL;
  Video::xapplicationname = NULL;

  Video::xvisible = 0;
  Video::yvisible = 0;

  Video::xwidth = 320;
  Video::xheight = 200;
  Video::xborderwidth = 0;
  Video::xpos = 0;
  Video::ypos = 0;

  Video::backgroundpixmap = 0;
  Video::drawpixmap = 0;
  Video::offscreenpixmap = 0;

  Video::maskpixmap = 0;
  Video::tmpmaskpixmap = 0;

  Video::interface = NULL;
  Video::videoOn = false;

  Video::xdbe = false;

  Video::empty = true;
}

Video::~Video() {

  if (Video::videoOn)
    Video::videoStop();
}

int Video::videoStart(int argc, char *argv[], char **envp) {

  if (Video::interface->messagesOn) {
    fprintf(Video::interface->messageFile, "Video...........starting\n");
    fprintf(Video::interface->messageFile, "Video...........using Xlib\n");
  }

  Video::xdisplay = XOpenDisplay(NULL);
  if (Video::xdisplay == NULL) {
    Video::videoOn = false;
    return (1);
  }
  if (Video::interface->messagesOn)
    fprintf(Video::interface->messageFile,
            "    Xlib........running on machine %s at %s\n", getenv("HOST"),
            XDisplayName((char *)Video::xdisplay));

  Video::videoFD = ConnectionNumber(Video::xdisplay);

  Video::xpos = (DisplayWidth(Video::xdisplay, DefaultScreen(Video::xdisplay)) -
                 Video::xwidth) /
                2;
  Video::ypos =
      (DisplayHeight(Video::xdisplay, DefaultScreen(Video::xdisplay)) -
       Video::xheight) /
      2;

  int xdbe_major_version, xdbe_minor_version;
  if (XdbeQueryExtension(Video::xdisplay, &xdbe_major_version,
                         &xdbe_minor_version) == 0) {
    Video::xdbe = false;
    if (Video::interface->Interface::messagesOn)
      fprintf(Video::interface->messageFile,
              "    Xlib........Double buffering not supported\n");
  } else {
    Video::xdbe = true;
    if (Video::interface->Interface::messagesOn)
      fprintf(Video::interface->messageFile,
              "    Xlib........Double buffering version %d.%d supported\n",
              xdbe_major_version, xdbe_minor_version);
  }

  XSetWindowAttributes xwindowattributes;
  xwindowattributes.save_under = true;
  xwindowattributes.override_redirect = false;
  xwindowattributes.event_mask = StructureNotifyMask | KeyPressMask |
                                 KeyReleaseMask | ExposureMask |
                                 FocusChangeMask;

  xwindowattributes.do_not_propagate_mask = false;
  unsigned int xwindowmask =
      CWSaveUnder | CWOverrideRedirect | CWEventMask | CWDontPropagate;

  Video::xwindow = XCreateWindow(
      Video::xdisplay, DefaultRootWindow(Video::xdisplay), Video::xpos,
      Video::ypos, Video::xwidth, Video::xheight, Video::xborderwidth,
      DefaultDepth(Video::xdisplay, DefaultScreen(Video::xdisplay)),
      InputOutput,
      DefaultVisual(Video::xdisplay, DefaultScreen(Video::xdisplay)),
      xwindowmask, &xwindowattributes);

  XSizeHints xsizehints;
  xsizehints.flags = PPosition | PSize;
  xsizehints.x = Video::xpos;
  xsizehints.y = Video::ypos;
  xsizehints.width = Video::xwidth;
  xsizehints.height = Video::xheight;

  XWMHints xwmhints;
  xwmhints.flags = StateHint | InputHint;
  xwmhints.initial_state = NormalState;
  xwmhints.input = true;

  Image *iconImage;
  if (Video::xicon != NULL) {
    if ((iconImage = (Image *)Video::createVideoImageFromFile(Video::xicon)) !=
        NULL) {
      xwmhints.icon_pixmap = iconImage->pixmap;
      xwmhints.icon_mask = iconImage->maskpixmap;
      xwmhints.flags |= IconPixmapHint | IconMaskHint;
    } else {
      if (Video::interface->Interface::messagesOn)
        fprintf(Video::interface->messageFile,
                "    Xlib........failed to create icon from file \'%s\'\n",
                Video::xicon);
    }
  }

  XClassHint xclasshint;
  xclasshint.res_class = Video::xapplicationclass;
  xclasshint.res_name = Video::xapplicationname;

  XTextProperty xwindowname, xiconname;
  if (XStringListToTextProperty(&(Video::xwindowtitle), 1, &xwindowname) == 0) {
    Video::videoOn = false;
    return (1);
  }
  if (XStringListToTextProperty(&(Video::xicontitle), 1, &xiconname) == 0) {
    Video::videoOn = false;
    return (1);
  }

  XSetWMProperties(Video::xdisplay, Video::xwindow, &xwindowname, &xiconname,
                   argv, argc, &xsizehints, &xwmhints, &xclasshint);

  XFree(xwindowname.value);
  XFree(xiconname.value);

  if (Video::xdbe) {

    Video::xdbebackbuffer = XdbeAllocateBackBufferName(
        Video::xdisplay, Video::xwindow, XdbeUndefined);
    Video::xdbeswapinfo.swap_window = Video::xwindow;
    Video::xdbeswapinfo.swap_action = XdbeUndefined;
  }

  XSetGraphicsExposures(
      Video::xdisplay,
      DefaultGC(Video::xdisplay, DefaultScreen(Video::xdisplay)), false);

  XMapRaised(Video::xdisplay, Video::xwindow);
  while (true) {
    XNextEvent(Video::xdisplay, &(Video::xevent));
    if (Video::xevent.type == MapNotify)
      break;
  }

  XAutoRepeatOff(Video::xdisplay);

  Video::videoOn = true;
  if (Video::interface->Interface::messagesOn)
    fprintf(Video::interface->messageFile, "    Xlib........window raised\n");

  return (0);
}

void Video::videoStop() {

  if (Video::xwindowtitle != NULL)
    delete Video::xwindowtitle;

  if (Video::xicontitle != NULL)
    delete Video::xicontitle;

  if (Video::xapplicationclass != NULL)
    delete Video::xapplicationclass;

  if (Video::xapplicationname != NULL)
    delete Video::xapplicationname;

  Image *imageID;
  while (Video::imageIDs != NULL) {
    XFreePixmap(Video::xdisplay, Video::imageIDs->pixmap);
    XFreePixmap(Video::xdisplay, Video::imageIDs->maskpixmap);
    imageID = Video::imageIDs->previous;
    free(Video::imageIDs);
    Video::imageIDs = imageID;
  }

  if (Video::drawpixmap != 0)
    XFreePixmap(Video::xdisplay, Video::drawpixmap);

  if (Video::offscreenpixmap != 0)
    XFreePixmap(Video::xdisplay, Video::offscreenpixmap);

  if (Video::maskpixmap != 0)
    XFreePixmap(Video::xdisplay, Video::maskpixmap);

  if (Video::tmpmaskpixmap != 0)
    XFreePixmap(Video::xdisplay, Video::tmpmaskpixmap);

  XFreeGC(Video::xdisplay, unmaskGC);
  XFreeGC(Video::xdisplay, maskGC);
  XFreeGC(Video::xdisplay, drawGC);

  if (Video::xdbe)
    XdbeDeallocateBackBufferName(Video::xdisplay, Video::xdbebackbuffer);

  XAutoRepeatOn(Video::xdisplay);

  XDestroyWindow(Video::xdisplay, Video::xwindow);
  if (Video::interface->messagesOn)
    fprintf(Video::interface->messageFile,
            "    Xlib........window destroyed\n");

  XCloseDisplay(Video::xdisplay);

  Video::videoOn = false;

  if (Video::interface->messagesOn)
    fprintf(Video::interface->messageFile, "Video...........stopped\n");
}

void *Video::createVideoImageFromFile(const char *filename) {

  if (Video::interface->messagesOn)
    fprintf(Video::interface->messageFile,
            "Video...........creating image from file \'%s\'\n", filename);

  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    if (Video::interface->messagesOn)
      fprintf(Video::interface->messageFile,
              "Video...........cannot open file \'%s\'\n", filename);
    return (NULL);
  }

  png_byte header[8];
  size_t size = fread(header, sizeof(png_byte), 8, fp);
  if (size <= 0 || !png_check_sig(header, 8)) {
    fclose(fp);
    if (Video::interface->messagesOn)
      fprintf(Video::interface->messageFile,
              "Video...........failed to create image, currently only \'png\' "
              "files are supported\n");
    return (NULL);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr) {
    fclose(fp);
    return (NULL);
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
    fclose(fp);
    return (NULL);
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (!end_info) {
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    fclose(fp);
    return (NULL);
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return (NULL);
  }

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 8);
  png_read_info(png_ptr, info_ptr);

  png_uint_32 width, height;
  int bit_depth, color_type, interlace_type;
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
               &interlace_type, int_p_NULL, int_p_NULL);

  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_expand(png_ptr);
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand(png_ptr);
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_expand(png_ptr);

  if (bit_depth == 16)
    png_set_strip_16(png_ptr);
  if (color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png_ptr);
  png_set_invert_alpha(png_ptr);

  double gamma, display_exponent = 2.2 * 1.0;

  if (png_get_gAMA(png_ptr, info_ptr, &gamma))
    png_set_gamma(png_ptr, display_exponent, gamma);

  if (color_type == PNG_COLOR_TYPE_RGB ||
      color_type == PNG_COLOR_TYPE_RGB_ALPHA &&
          DefaultDepth(Video::xdisplay, DefaultScreen(Video::xdisplay)))
    png_set_bgr(png_ptr);

  png_read_update_info(png_ptr, info_ptr);

  png_uint_32 rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  unsigned char *image_data;
  if ((image_data = new unsigned char[rowbytes * height]) == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return (NULL);
  }

  png_bytep *row_pointers;

  if ((row_pointers = new png_bytep[height]) == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return (NULL);
  }

  for (png_uint_32 i = 0; i < height; ++i)
    row_pointers[i] = image_data + i * rowbytes;

  png_read_image(png_ptr, row_pointers);

  png_read_end(png_ptr, end_info);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

  delete row_pointers;

  fclose(fp);

  XImage *ximage = XCreateImage(
      Video::xdisplay,
      DefaultVisual(Video::xdisplay, DefaultScreen(Video::xdisplay)),
      DefaultDepth(Video::xdisplay, DefaultScreen(Video::xdisplay)), ZPixmap, 0,
      (char *)image_data, width, height, bit_depth, rowbytes);

  //  if(ImageByteOrder(Video::xdisplay) == LSBFirst)
  ximage->byte_order = LSBFirst;
  //  else
  // ximage->byte_order = MSBFirst;

  Image *imageID = new Image;

  imageID->width = ximage->width;
  imageID->height = ximage->height;

  imageID->pixmap = XCreatePixmap(
      Video::xdisplay, Video::xwindow, ximage->width, ximage->height,
      DefaultDepth(Video::xdisplay, DefaultScreen(Video::xdisplay)));
  imageID->maskpixmap = XCreatePixmap(Video::xdisplay, Video::xwindow,
                                      ximage->width, ximage->height, 1);

  imageID->previous = Video::imageIDs;

  if (imageID->previous != NULL)
    imageID->previous->next = imageID;

  imageID->next = NULL;

  Video::imageIDs = imageID;

  XPutImage(Video::xdisplay, imageID->pixmap,
            DefaultGC(Video::xdisplay, DefaultScreen(Video::xdisplay)), ximage,
            0, 0, 0, 0, ximage->width, ximage->height);

  if (Video::interface->messagesOn)
    fprintf(Video::interface->messageFile,
            "Video...........image dimensions %d x %d\n", ximage->width,
            ximage->height);

  if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {

    XGCValues xgcvalues;
    xgcvalues.function = GXcopy;
    xgcvalues.graphics_exposures = false;
    xgcvalues.foreground = 1;
    xgcvalues.plane_mask = AllPlanes;
    unsigned long xgcvalues_mask =
        GCFunction | GCGraphicsExposures | GCForeground | GCPlaneMask;

    GC alphaGC = XCreateGC(Video::xdisplay, imageID->maskpixmap, xgcvalues_mask,
                           &xgcvalues);

    XFillRectangle(Video::xdisplay, imageID->maskpixmap, alphaGC, 0, 0,
                   ximage->width, ximage->height);
    XSetForeground(Video::xdisplay, alphaGC, 0);

    char alpha;
    for (int y = 0; y < ximage->height; y++) {
      for (int x = 0; x < ximage->width; x++) {
        alpha = ximage->data[3 + (y * ximage->width + x) * 4];
        if (alpha < 0)
          XDrawPoint(Video::xdisplay, imageID->maskpixmap, alphaGC, x, y);
      }
    }
    XFreeGC(Video::xdisplay, alphaGC);
  }

  XDestroyImage(ximage);

  return (imageID);
}

int Video::getVideoInput(int **keysPressed, int **keysDown) {

  KeySym xkey;

  int n_keysPressed = 0, n_keysDown = 0, sentinal, quit = 0, downkeys = 0;

  int *act = NULL;
  if (*keysPressed != NULL) {

    act = *keysPressed;
    while (*act++ != XK_VoidSymbol) {
      n_keysPressed++;
      act++;
    }
  }

  if (keysDown == NULL) {

    downkeys = 1;
    keysDown = (int **)malloc(sizeof(int **));
    *keysDown = NULL;
  }

  if (*keysDown != NULL) {

    act = *keysDown;
    while (*act != XK_VoidSymbol) {
      n_keysDown++;
      act++;
    }
  }

  while (XPending(Video::xdisplay) > 0) {

    XNextEvent(Video::xdisplay, &xevent);
    switch (Video::xevent.type) {

    case KeyPress:

      xkey = XkbKeycodeToKeysym(Video::xdisplay, xevent.xkey.keycode, 0,
                                xevent.xkey.state & ShiftMask ? 1 : 0);

      sentinal = n_keysDown;
      while (sentinal--) {
        if ((*keysDown)[sentinal] == (int)xkey)
          break;
      }

      if (sentinal == -1) {

        switch (xkey) {

        default:

          *keysDown = (int *)realloc(*keysDown, sizeof(int) * (n_keysDown + 1));
          (*keysDown)[n_keysDown++] = (int)xkey;
          break;

        case XK_Escape:
          quit = 1;
          break;
        };

        sentinal = n_keysPressed;
        while (sentinal--) {
          if ((*keysPressed)[sentinal] == (int)xkey)
            break;
        }

        if (sentinal == -1) {
          *keysPressed =
              (int *)realloc(*keysPressed, sizeof(int) * (n_keysPressed + 1));
          (*keysPressed)[n_keysPressed++] = (int)xkey;
        }
      }
      break;

    case KeyRelease:

      xkey = XkbKeycodeToKeysym(Video::xdisplay, xevent.xkey.keycode, 0,
                                xevent.xkey.state & ShiftMask ? 1 : 0);

      sentinal = n_keysDown;
      while (sentinal--) {
        if ((*keysDown)[sentinal] == (int)xkey) {

          n_keysDown--;
          memmove((*keysDown) + sentinal, (*keysDown) + sentinal + 1,
                  sizeof(int) * (n_keysDown - sentinal));
        }
      }

      break;

    case NoExpose:
      break;

    case Expose:
      Video::videoSync();
      break;

    case FocusIn:
      XAutoRepeatOff(Video::xdisplay);
      break;

    case FocusOut:
      XAutoRepeatOn(Video::xdisplay);
      break;

    default:
      break;
    };
  }

  *keysPressed =
      (int *)realloc(*keysPressed, sizeof(int) * (n_keysPressed + 1));
  (*keysPressed)[n_keysPressed] = XK_VoidSymbol;

  if (downkeys == 0) {
    *keysDown = (int *)realloc(*keysDown, sizeof(int) * (n_keysDown + 1));
    (*keysDown)[n_keysDown] = XK_VoidSymbol;
  } else {
    free(*keysDown);
    free(keysDown);
  }

  return (quit);
}

void Video::freeVideoImage(void *imageID) {

  Image *ID = (Image *)imageID;

  XFreePixmap(Video::xdisplay, ID->pixmap);
  XFreePixmap(Video::xdisplay, ID->maskpixmap);

  if (ID->next != NULL)
    ID->next->previous = ID->previous;
  if (ID->previous != NULL)
    ID->previous->next = ID->next;

  if (Video::imageIDs == ID)
    Video::imageIDs = ID->previous;

  free(ID);
}

void Video::drawVideoImage(void *imageID, int src_x, int src_y, int width,
                           int height, int dest_x, int dest_y) {

  Image *ID = (Image *)imageID;

  if (!Video::empty)
    XSetFunction(Video::xdisplay, Video::maskGC, GXor);
  else
    Video::empty = false;

  XCopyArea(Video::xdisplay, ID->maskpixmap, Video::maskpixmap, Video::maskGC,
            src_x, src_y, width, height, dest_x, dest_y);

  XCopyArea(Video::xdisplay, ID->maskpixmap, Video::tmpmaskpixmap,
            Video::maskGC, src_x, src_y, width, height, dest_x, dest_y);
  XSetClipMask(Video::xdisplay, Video::drawGC, Video::tmpmaskpixmap);

  XCopyArea(Video::xdisplay, ID->pixmap, Video::drawpixmap, Video::drawGC,
            src_x, src_y, width, height, dest_x, dest_y);

  XSetFunction(Video::xdisplay, Video::maskGC, GXcopy);
  XFillRectangle(Video::xdisplay, Video::tmpmaskpixmap, Video::maskGC, dest_x,
                 dest_y, width, height);
}

void Video::copyVideoImage(void *imageIDsrc, void *imageIDdst, int src_x,
                           int src_y, int width, int height, int dest_x,
                           int dest_y) {

  Image *IDsrc = (Image *)imageIDsrc;
  Image *IDdst = (Image *)imageIDdst;

  XCopyArea(Video::xdisplay, IDsrc->maskpixmap, IDdst->maskpixmap,
            Video::unmaskGC, src_x, src_y, width, height, dest_x, dest_y);
  XSetClipMask(Video::xdisplay, Video::drawGC, IDdst->maskpixmap);
  XCopyArea(Video::xdisplay, IDsrc->pixmap, IDdst->pixmap, Video::drawGC, src_x,
            src_y, width, height, dest_x, dest_y);
}

void Video::drawVideoString(size_t len, char *str, int dest_x, int dest_y) {

  XDrawString(Video::xdisplay, Video::maskpixmap, unmaskGC, dest_x, dest_y, str,
              len);
  XSetClipMask(Video::xdisplay, Video::drawGC, Video::maskpixmap);
  XDrawString(Video::xdisplay, Video::drawpixmap, Video::drawGC, dest_x, dest_y,
              str, len);
}

void Video::setVideoBackgroundImage(void *imageID) {

  int width = Video::getVideoImageWidth(imageID),
      height = Video::getVideoImageHeight(imageID);

  Video::backgroundpixmap = XCreatePixmap(
      Video::xdisplay, Video::xwindow, width, height,
      DefaultDepth(Video::xdisplay, DefaultScreen(Video::xdisplay)));

  if (Video::drawpixmap != 0)
    XFreePixmap(Video::xdisplay, Video::drawpixmap);

  Video::drawpixmap = XCreatePixmap(
      Video::xdisplay, Video::xwindow, width, height,
      DefaultDepth(Video::xdisplay, DefaultScreen(Video::xdisplay)));

  if (Video::offscreenpixmap != 0) {

    XFreePixmap(Video::xdisplay, Video::offscreenpixmap);

    XFreeGC(Video::xdisplay, Video::copyGC);
  }

  Video::offscreenpixmap = XCreatePixmap(
      Video::xdisplay, Video::xwindow, width, height,
      DefaultDepth(Video::xdisplay, DefaultScreen(Video::xdisplay)));

  if (Video::maskpixmap != 0) {

    XFreePixmap(Video::xdisplay, Video::maskpixmap);

    XFreeGC(Video::xdisplay, Video::maskGC);
    XFreeGC(Video::xdisplay, Video::unmaskGC);
    XFreeGC(Video::xdisplay, Video::drawGC);
  }

  Video::maskpixmap =
      XCreatePixmap(Video::xdisplay, Video::xwindow, width, height, 1);

  if (Video::tmpmaskpixmap != 0)
    XFreePixmap(Video::xdisplay, Video::tmpmaskpixmap);

  Video::tmpmaskpixmap =
      XCreatePixmap(Video::xdisplay, Video::xwindow, width, height, 1);

  XGCValues xgcvalues;
  xgcvalues.foreground = 0;
  xgcvalues.graphics_exposures = false;
  xgcvalues.function = GXcopy;

  unsigned long xgcvalues_mask = GCForeground | GCGraphicsExposures;

  Video::maskGC =
      XCreateGC(Video::xdisplay, Video::maskpixmap, xgcvalues_mask, &xgcvalues);
  xgcvalues.foreground = 1;
  Video::unmaskGC =
      XCreateGC(Video::xdisplay, Video::maskpixmap, xgcvalues_mask, &xgcvalues);

  XColor xcolor;
  char *color = TEXT_COLOR;

  XParseColor(Video::xdisplay,
              DefaultColormap(Video::xdisplay, DefaultScreen(Video::xdisplay)),
              color, &xcolor);
  XAllocColor(Video::xdisplay,
              DefaultColormap(Video::xdisplay, DefaultScreen(Video::xdisplay)),
              &xcolor);

  xgcvalues.foreground = xcolor.pixel;
  Video::drawGC =
      XCreateGC(Video::xdisplay, Video::drawpixmap, xgcvalues_mask, &xgcvalues);

  xgcvalues.function = GXcopy;
  Video::copyGC = XCreateGC(Video::xdisplay, Video::offscreenpixmap,
                            xgcvalues_mask, &xgcvalues);

  XFillRectangle(Video::xdisplay, Video::maskpixmap, Video::maskGC, 0, 0, width,
                 height);
  XFillRectangle(Video::xdisplay, Video::tmpmaskpixmap, Video::maskGC, 0, 0,
                 width, height);

  XFillRectangle(Video::xdisplay, Video::offscreenpixmap, Video::copyGC, 0, 0,
                 width, height);

  Image *ID = (Image *)imageID;
  Video::backgroundpixmap = ID->pixmap;
}

void Video::videoSync() {

  Video::empty = true;

  XCopyArea(Video::xdisplay, Video::backgroundpixmap, Video::offscreenpixmap,
            Video::copyGC, Video::xvisible, Video::yvisible, Video::xwidth,
            Video::xheight, Video::xvisible, Video::yvisible);

  XSetClipMask(Video::xdisplay, Video::drawGC, Video::maskpixmap);
  XCopyArea(Video::xdisplay, Video::drawpixmap, Video::offscreenpixmap,
            Video::drawGC, Video::xvisible, Video::yvisible, Video::xwidth,
            Video::xheight, Video::xvisible, Video::yvisible);

  if (Video::xdbe) {
    XCopyArea(Video::xdisplay, Video::offscreenpixmap, Video::xdbebackbuffer,
              Video::copyGC, Video::xvisible, Video::yvisible, Video::xwidth,
              Video::xheight, 0, 0);
    XdbeSwapBuffers(Video::xdisplay, &(Video::xdbeswapinfo), 1);
  } else
    XCopyArea(Video::xdisplay, Video::offscreenpixmap, Video::xwindow,
              Video::copyGC, Video::xvisible, Video::yvisible, Video::xwidth,
              Video::xheight, 0, 0);

  XFillRectangle(Video::xdisplay, Video::maskpixmap, Video::maskGC,
                 Video::xvisible, Video::yvisible, Video::xwidth,
                 Video::xheight);
  XSetClipMask(Video::xdisplay, Video::drawGC, Video::maskpixmap);

  XFlush(Video::xdisplay);

  nanosleep(&(Video::sleep), NULL);
}
