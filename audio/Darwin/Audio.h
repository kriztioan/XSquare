/**
 *  @file   Audio.h
 *  @brief  MacOS Audio Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-26
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef AUDIO_H_
#define AUDIO_H_

#include <AudioUnit/AudioUnit.h>
#include <CoreAudio/CoreAudio.h>
#include <errno.h>

inline OSStatus OutsideRenderer(void *inRefCon,
                                AudioUnitRenderActionFlags *ioActionFlags,
                                const AudioTimeStamp *inTimeStamp,
                                UInt32 inBusNumber, UInt32 inNumberFrames,
                                AudioBufferList *ioData);

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

  void playSound(char sample, char loop = 0);

  OSStatus audioRenderer(void *inRefCon,
                         AudioUnitRenderActionFlags *ioActionFlags,
                         const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
                         UInt32 inNumberFrames, AudioBufferList *ioData);

protected:
  Interface *interface;

  bool mute;

private:
  AudioComponent output_comp;

  AudioComponentInstance output_instance;

  bool audioOn;

  unsigned int samplerate;
  unsigned int channels;
  unsigned int stereo;
  unsigned int gain;

  typedef struct {
    int size;
    char *data;
  } Sample;

  Sample *samples;
  int n_samples;

  typedef struct {
    int pos;
    char sample;
    char loop;
  } AudioChannel;

  AudioChannel *mixer;
};

#include "Interface.h"

#endif // End of AUDIO_H_
