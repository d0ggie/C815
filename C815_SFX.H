#ifndef __C815_SFX_H
#define __C815_SFX_H

typedef int8_t
  GAudio__SamplePCM_t;

enum
{
// Note: This is the sample rate which is programmed for Soundblaster and it is
// indeed a common one.  Gravis Ultrasound configuration has a slight bug.  GUS
// sample rate depends on  a number enabled voices,  or active channels.  There
// apparently weren't any official documentation, so details are mostly reverse
// engineered based on the Gravis SDK.  DOSBox uses  the following equation for
// base frequency:
//   <base-frequency> = ``0.5  +  1000000.0  / (1.619695497 * <num-voices>)''.
// The game uses 27 voices.  That however yields  ~ 22868 Hz.  The game  should
// have used  28  voices,  as that brings  base frequency down to  22050 Hz.  A
// divisor to get final samplerate <samplerate> is calculated as follows:
//   <divisor> = ``<sample-rate> * 1024 / <base-frequency> + 1''.
// The game uses 372 as the divisor value.  With  the (incorrect) 22868 Hz base
// frequency this means a sample rate of ~ 8285 Hz.  To make things worse,  the
// divisor value isn't actually correct, either.  It should be 373, as the used
// value is not rounded correctly (372.5193), to minimize the frequency error.
  GAUDIO__SAMPLERATE =
#ifdef C815__GUS_PURITIST
    8285
#else
    8000
#endif // ..
, GAUDIO__BITRATE =
    bsizof(
  GAudio__SamplePCM_t)
, GAUDIO__CHANNELS = 1
};

typedef struct
__GAudio__SoundPCM
{
  GAudio__SamplePCM_t *
    data;
  uint32_t
    size;
} GAudio__SoundPCM_t;

extern
void
GAudio__DumpSound(
  const
  GAudio__SoundPCM_t *
  const
    gAudio__SoundPCM,
  unsigned
    gAudio__SoundPCM__Indx);

extern
void
GAudio__LoadSound(
  GAudio__SoundPCM_t *
  const
    gAudio__SoundPCM,

  FILE *
  const
    file__hndl
  );

extern
void
GAudio__Free(
  GAudio__SoundPCM_t *
  const
    gAudio__SoundPCM
  );

#endif//__C815_SFX_H