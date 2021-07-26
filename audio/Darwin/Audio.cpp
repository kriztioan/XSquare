/**
 *  @file   Audio.h
 *  @brief  MacOS Audio Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-26
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Audio.h"

Audio::Audio() { Audio::interface = NULL; }

Audio::~Audio() {

  if (Audio::audioOn)
    Audio::audioStop();
}

void Audio::audioClear() {

  if (Audio::mixer)
    memset(Audio::mixer, 0, Audio::channels * sizeof(AudioChannel));

  if (Audio::samples) {

    for (int i = 0; i < Audio::n_samples; i++) {

      if (samples[i].data)
        free(samples[i].data);
    }

    Audio::n_samples = 0;
    Audio::samples = NULL;
  }
}

void Audio::audioStop() {

  if (Audio::audioOn) {

    AudioComponentInstanceDispose(Audio::output_instance);

    AudioOutputUnitStop(Audio::output_instance);

    Audio::audioOn = false;
  }

  Audio::audioClear();

  if (Audio::mixer)
    free(Audio::mixer);

  if (Audio::interface->messagesOn)
    fprintf(Audio::interface->messageFile, "Audio...........stopped\n");
}

void Audio::audioStart() {

  if (Audio::interface->messagesOn)
    fprintf(Audio::interface->messageFile, "Audio...........starting\n");

  AudioComponentDescription desc;

  desc.componentType = kAudioUnitType_Output;
  desc.componentSubType = kAudioUnitSubType_DefaultOutput;
  desc.componentFlags = 0;
  desc.componentFlagsMask = 0;
  desc.componentManufacturer = kAudioUnitManufacturer_Apple;

  Audio::output_comp = AudioComponentFindNext(NULL, &desc);

  if (!Audio::output_comp) {
    fprintf(Audio::interface->messageFile, "Audio...........failed (1)\n");
    return;
  }

  if (AudioComponentInstanceNew(output_comp, &output_instance)) {
    fprintf(Audio::interface->messageFile, "Audio...........failed (2)\n");
    return;
  }

  if (AudioUnitInitialize(output_instance)) {
    fprintf(Audio::interface->messageFile, "Audio...........failed (3)\n");
    return;
  }

  AudioStreamBasicDescription streamFormat;
  UInt32 size = sizeof(AudioStreamBasicDescription);

  if (AudioUnitGetProperty(output_instance, kAudioUnitProperty_StreamFormat,
                           kAudioUnitScope_Input, 0, &streamFormat, &size)) {
    fprintf(Audio::interface->messageFile, "Audio...........failed (4)\n");
    return;
  }

  streamFormat.mSampleRate = Audio::samplerate;
  streamFormat.mFormatID = kAudioFormatLinearPCM;
  streamFormat.mFormatFlags =
      kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian;
  streamFormat.mFramesPerPacket = 1;
  streamFormat.mChannelsPerFrame = Audio::stereo;
  streamFormat.mBitsPerChannel = Audio::channels;

  if (AudioUnitSetProperty(output_instance, kAudioUnitProperty_StreamFormat,
                           kAudioUnitScope_Input, 0, &streamFormat,
                           sizeof(streamFormat))) {
    fprintf(Audio::interface->messageFile, "Audio...........failed (5)\n");
    return;
  }

  AURenderCallbackStruct renderer;
  renderer.inputProc = OutsideRenderer;
  renderer.inputProcRefCon = this;

  if (AudioUnitSetProperty(output_instance,
                           kAudioUnitProperty_SetRenderCallback,
                           kAudioUnitScope_Output, 0, &renderer,
                           sizeof(AURenderCallbackStruct))) {
    fprintf(Audio::interface->messageFile, "Audio...........failed (6)\n");
    return;
  }

  if (AudioOutputUnitStart(output_instance)) {
    fprintf(Audio::interface->messageFile, "Audio...........failed (7)\n");
    return;
  }

  Audio::audioOn = true;

  Audio::samples = NULL;
  Audio::n_samples = 0;

  Audio::mixer = (AudioChannel *)calloc(Audio::channels, sizeof(AudioChannel));

  Audio::mute = false;
}

void Audio::setAudioSamplerate(unsigned int rate) {

  if (Audio::interface->messagesOn)
    fprintf(Audio::interface->messageFile,
            "Audio...........setting samplerate to %d Hz\n", rate);

  Audio::samplerate = rate;
}

void Audio::setAudioChannels(unsigned int channels) {

  if (Audio::interface->messagesOn)
    fprintf(Audio::interface->messageFile,
            "Audio...........setting channels to %d\n", channels);

  Audio::channels = channels;
}

void Audio::setAudioStereo(unsigned int stereo) {

  if (Audio::interface->messagesOn) {
    if (stereo == 2)
      fprintf(Audio::interface->messageFile,
              "Audio...........setting output to stereo\n");
    else
      fprintf(Audio::interface->messageFile,
              "Audio...........setting output to mono\n");
  }
  Audio::stereo = stereo;
}

void Audio::setAudioGain(unsigned int gain) {

  if (Audio::interface->messagesOn)
    fprintf(Audio::interface->messageFile,
            "Audio...........setting gain to %d\n", gain);
  Audio::gain = gain;
}

void Audio::loadSoundFromFile(const char *filename) {

  int fd = open(filename, O_RDONLY), size;

  if (fd == -1) {
    if (Audio::interface->messagesOn)
      fprintf(Audio::interface->messageFile,
              "Audio...........loading samples from file \'%s\' failed\n",
              filename);
    return;
  }

  Audio::samples = (Sample *)realloc(Audio::samples,
                                     (Audio::n_samples + 1) * sizeof(Sample));

  Audio::samples[Audio::n_samples].size = size = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);

  samples[Audio::n_samples].data =
      new char[Audio::samples[Audio::n_samples].size];

  if (samples[Audio::n_samples].data != NULL) {

    read(fd, samples[Audio::n_samples].data,
         Audio::samples[Audio::n_samples].size);

    char *tmp = samples[Audio::n_samples].data;
    while (size--) {

      *tmp -= 127;
      *tmp++ /= Audio::gain;
    }
  } else
    samples[Audio::n_samples].data = NULL;
  close(fd);

  if (Audio::interface->messagesOn)
    fprintf(Audio::interface->messageFile,
            "Audio...........loaded samples from file \'%s\', %d bytes\n",
            filename, Audio::samples[Audio::n_samples].size);

  ++Audio::n_samples;
}

void Audio::playSound(char sample, char loop) {

  if (!Audio::mute && Audio::mixer && (int)sample < Audio::n_samples &&
      samples[sample].data) {

    for (int i = 0; i < Audio::channels; i++) {

      if (!Audio::mixer[i].pos && !Audio::mixer[i].loop) {

        Audio::mixer[i].pos = 1;
        Audio::mixer[i].sample = sample;
        Audio::mixer[i].loop = loop;

        break;
      }
    }
  }
}

void Audio::toggleAudioMute() {

  Audio::mute = !(Audio::mute);
  if (Audio::mute)
    AudioOutputUnitStop(Audio::output_instance);
  else
    AudioOutputUnitStart(Audio::output_instance);
}

OSStatus Audio::audioRenderer(void *inRefCon,
                              AudioUnitRenderActionFlags *ioActionFlags,
                              const AudioTimeStamp *inTimeStamp,
                              UInt32 inBusNumber, UInt32 inNumberFrames,
                              AudioBufferList *ioData) {

  int bytes_to_copy;
  char *buf = (char *)ioData->mBuffers[0].mData, *p, *pp;

  memset(buf, 0, ioData->mBuffers[0].mDataByteSize);

  if (!Audio::mixer || !Audio::samples)
    return noErr;

  for (int i = 0; i < Audio::channels; i++) {

    if (!Audio::mixer[i].pos)
      continue;

    if (Audio::mixer[i].pos == Audio::samples[Audio::mixer[i].sample].size) {

      if (mixer[i].loop)
        mixer[i].pos = 1;
      else {
        mixer[i].pos = 0;

        continue;
      }
    }

    p = Audio::samples[Audio::mixer[i].sample].data;

    if (!p)
      continue;

    p += Audio::mixer[i].pos - 1;

    if ((Audio::mixer[i].pos + ioData->mBuffers[0].mDataByteSize) <=
        Audio::samples[Audio::mixer[i].sample].size)
      bytes_to_copy = ioData->mBuffers[0].mDataByteSize;
    else
      bytes_to_copy =
          Audio::samples[Audio::mixer[i].sample].size - Audio::mixer[i].pos;

    Audio::mixer[i].pos += bytes_to_copy;

    pp = buf;

    while (bytes_to_copy--) {
      *pp = *pp + *p - *pp * *p / 255; // avoid clipping
      pp++;
      p++;
    }
  }

  return noErr;
}

OSStatus OutsideRenderer(void *inRefCon,
                         AudioUnitRenderActionFlags *ioActionFlags,
                         const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
                         UInt32 inNumberFrames, AudioBufferList *ioData) {

  Audio *obj = (Audio *)inRefCon;

  return obj->audioRenderer(inRefCon, ioActionFlags, inTimeStamp, inBusNumber,
                            inNumberFrames, ioData);
}
