/**
 *  @file   Audio.h
 *  @brief  Linux Audio Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-27
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef AUDIO_H_
#define AUDIO_H_

#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <linux/soundcard.h>

#define SOUND_CLEAR -3
#define SOUND_EXIT -2
#define SOUND_CONT -1
#define SOUND_ONCE 0

class Interface;

class Audio {

public:
  Audio();
  ~Audio();

  void audioStart();
  void audioStop();
  void audioClear();

  void setAudioSamplerate(unsigned int rate);
  void setAudioChannels(unsigned int channels);
  void setAudioStereo(unsigned int stereo);
  void setAudioGain(unsigned int gain);

  void toggleAudioMute();

  void loadSoundFromFile(const char *filename);

  void playSound(char sample, char cont = 0);

protected:
  Interface *interface;

  bool mute;

private:
  char *device;

  int dsp;
  int blocksize;
  int readfd;
  int writefd;

  bool audioOn;

  unsigned int samplerate;
  unsigned int channels;
  unsigned int stereo;
  unsigned int gain;

  typedef struct {
    size_t size;
    char *data;
  } Sample;

  Sample *samples;
  int n_samples;

  typedef struct {
    size_t size;
    size_t pos;
    char *data;
    bool loop;
  } AudioChannel;

  AudioChannel *mixer;

  typedef struct {
    char type;
    size_t size;
  } AudioMessage;

  void audioServer();
};

inline void Audio::toggleAudioMute() { Audio::mute = !(Audio::mute); }

#include "Interface.h"

#endif // End of AUDIO_H_
