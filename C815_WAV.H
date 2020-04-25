#ifndef __C815_WAV_H
#define __C815_WAV_H

#include "C815_INT.H"
#include "C815_SFX.H"

#define \
RIFF__CHUNKID(_3, _2, _1, _0)                                                  \
  ((                                                                           \
((_3) << 0) | ((_2) << 8) | ((_1) << 16) | ((_0) << 24)                        \
  ))

typedef struct __attribute__((__packed__))
__RIFFHEADER
{
#define \
RIFF__RIFF                                                                     \
RIFF__CHUNKID('R', 'I', 'F', 'F')
  DWRD rix__ChunkID;
  DWRD rix__Size;
#define \
RIFF__WAVE                                                                     \
RIFF__CHUNKID('W', 'A', 'V', 'E')
  DWRD rix__Type;
} RIFFHEADER_t;

typedef struct __attribute__((__packed__))
__RIFFCHUNK
{
  DWRD rix__ChunkID;
  DWRD rix__Size;
  BYTE rix__Payload[0];
} RIFFCHUNK_t;
static_assert(sizeof(
  RIFFCHUNK_t) == 8);

enum
{
  WAVE_FORMAT_PCM = 1
};

typedef struct __attribute__((__packed__))
__WAVEFORMATEX
{
#define \
RIFF__WAVE_FMT                                                                 \
RIFF__CHUNKID('f', 'm', 't', ' ')
  WORD wav__Format;
  WORD wav__Channels;
  DWRD wav__Samplerate;
  DWRD wav__Byterate__Avg;
  WORD wav__BlockAlign;
  WORD wav__Bitrate;
  WORD wav__Size__Extra;
} WAVEFORMATEX_t;

#define \
RIFF__WAVE_DATA                                                                \
RIFF__CHUNKID('d', 'a', 't', 'a')

extern
void
GAudio__WriteWAV__Alt(
  const
  GAudio__SoundPCM_t *
  const
    gAudio__SoundPCM,

  FILE *
  const
    file__hndl
  );

extern
void
GAudio__WriteWAV(
  const
  GAudio__SoundPCM_t *
  const
    gAudio__SoundPCM,

  const
  char *
  const
    name__fmt, ...
  );


#endif// __C815_WAV_H
