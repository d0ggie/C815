#include <C815_STD.H>
#include "C815_WAV.H"
#include "C815_API.H"

static
void
GAudio__PCMTransform(
  GAudio__SamplePCM_t *
    gAudio__SoundPCM,
  DWRD
    gAudio__SoundPCM__Size
  )
  {
  const
  GAudio__SamplePCM_t *
  const
    gAudio__SoundPCM__Tail =
 &gAudio__SoundPCM[
    gAudio__SoundPCM__Size];

    // Note: For  `WAVE_FORMAT_PCM'  8-bit (and below)  PCM files are  unsigned
    // while  the game  (raw data)  uses signed  PCM samples.  As  both  Gravis
    // Ultrasound  and  Soundblaster use unsigned PCM samples the game performs
    // this very same transformation;  Though is simply directly flips  the MSB
    // as that is what really happens here,  perhaps the compiler does the same
    // for us.
  while (
    gAudio__SoundPCM
      <
    gAudio__SoundPCM__Tail)
      {
   *gAudio__SoundPCM +=
      1 << (bsizof(
    GAudio__SamplePCM_t) - 1);

      /* // Note: Not enabled.
   *gAudio__SoundPCM >>= 1 */
        ;
  ++gAudio__SoundPCM;
      }

  return;
  }

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
  )
  {
  const
  RIFFCHUNK_t
    wav__SoundPCM__CHUNK =
  {
   .rix__ChunkID =
  RIFF__WAVE_DATA
 , .rix__Size =
  gAudio__SoundPCM->size
  };

  const
  RIFFCHUNK_t
    wav__FormatEx__CHUNK =
  {
   .rix__ChunkID =
  RIFF__WAVE_FMT
 , .rix__Size =
      sizeof(
  WAVEFORMATEX_t)
  };

  const
  WAVEFORMATEX_t
    wav__FormatEx =
  {
   .wav__Format                 = WAVE_FORMAT_PCM
 , .wav__Channels               = GAUDIO__CHANNELS
 , .wav__Samplerate             = GAUDIO__SAMPLERATE
 , .wav__Byterate__Avg          = GAUDIO__SAMPLERATE * GAUDIO__CHANNELS
 , .wav__BlockAlign             = GAUDIO__SAMPLERATE * GAUDIO__BITRATE / 8
 , .wav__Bitrate                = GAUDIO__BITRATE
 , .wav__Size__Extra            = 0
  };

  const
  RIFFHEADER_t
    rix__Header =
  {
   .rix__ChunkID =
  RIFF__RIFF
 , .rix__Size =
    wav__SoundPCM__CHUNK
   .rix__Size +
    sizeof(
  RIFFCHUNK_t)
  + wav__FormatEx__CHUNK
   .rix__Size +
    sizeof(
  RIFFCHUNK_t)
  + sizeof(
  RIFFHEADER_t)
  - sizeof(
  RIFFCHUNK_t)

 , .rix__Type =
  RIFF__WAVE
  };

  FWRITE(
    rix__Header,
    file__hndl);
  FWRITE(
    wav__FormatEx__CHUNK,
    file__hndl);
  FWRITE(
    wav__FormatEx,
    file__hndl);
  FWRITE(
    wav__SoundPCM__CHUNK,
    file__hndl);

  const
  GAudio__SamplePCM_t *
  const
  __wav__SoundPCM =
  gAudio__SoundPCM->data;

  GAudio__SamplePCM_t * __attribute__((__cleanup__(GAlloc__FreeI8)))
    wav__SoundPCM =
  GAlloD(
    wav__SoundPCM__CHUNK.rix__Size,
  __wav__SoundPCM);
  GAudio__PCMTransform(
    wav__SoundPCM,
    wav__SoundPCM__CHUNK.rix__Size);

  fwrite__alt(
    wav__SoundPCM,
    wav__SoundPCM__CHUNK.rix__Size,
    file__hndl);

  return;
  }

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
  )
  {
  char
    name[128];
  va_list
    name__ap;
  va_start(
    name__ap,
    name__fmt);
  vsnprintf(
    name,
      sizeof(
    name),
    name__fmt,
    name__ap);
  va_end(
    name__ap);

  FILE * __attribute__((__cleanup__(GStdIO__Close)))
    file__hndl =
  fopen__alt(
    name, "wb");
GAudio__WriteWAV__Alt(
    gAudio__SoundPCM,
    file__hndl);

  return;
  }
