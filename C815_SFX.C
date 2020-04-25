#include <C815_STD.H>
#include "C815_SFX.H"
#include "C815_API.H"

extern
void
GAudio__DumpSound(
  const
  GAudio__SoundPCM_t *
  const
    gAudio__SoundPCM,
  unsigned
    gAudio__SoundPCM__Indx
  )
  {
  char
    name[80];
  snprintf(
    name,
      sizeof(
    name),
    "DUMP\\SNDOBJ%02X.RAW",
    gAudio__SoundPCM__Indx);

  FILE * __attribute__((__cleanup__(GStdIO__Close)))
    hndl__file =
  fopen(
    name, "wb");
  if (!!
    hndl__file)
  fwrite__alt(
    gAudio__SoundPCM->data,
    gAudio__SoundPCM->size,
    hndl__file);

  return;
  }

extern
void
GAudio__LoadSound(
  GAudio__SoundPCM_t *
  const
    gAudio__SoundPCM,
  FILE *
  const
    file__hndl
  )
  {
  static
  unsigned
    gAudio__Sound__N = 0;

  FREAD(
    gAudio__SoundPCM->size, file__hndl);
  gAudio__SoundPCM->data =
  GAlloc(
    gAudio__SoundPCM->size);
  fread__alt(
    gAudio__SoundPCM->data,
    gAudio__SoundPCM->size, file__hndl);

#if \
GAUDIO__LOADSOUND__TRANSFORM
  GAudio__SamplePCM_t *
  __gAudio__Sound__data =
  gAudio__SoundPCM->data;
  const
  GAudio__SamplePCM_t *
  __gAudio__Sound__tail =
 &__gAudio__Sound__data[
    gAudio__SoundPCM->size]
#endif // ..
    ;

#if \
GAUDIO__LOADSOUND__TRANSFORM
  while (
  __gAudio__Sound__data
      <
  __gAudio__Sound__tail)
    {
/*
0001067C                 xor     bl, 80h         ; Logical Exclusive OR
0001067F                 and     ebx, 0FFh       ; Logical AND
00010685                 sar     ebx, 1          ; Shift Arithmetic Right
*/
      // Note: This is effectively the same adding 128,  i.e. making the signed
      // number unsigned  (and zero level at 128).  This is done as both of the
      // supported sound cards operate on unsigned PCM samples.
 *__gAudio__Sound__data ^= 0x80;
   /* // Note: Not enabled -- practially a noop here.
 *__gAudio__Sound__data &= 0xff; */
      // Note: Simply attenuate by 3 dB.  I do not know why the game does this.
 *__gAudio__Sound__data++ >>= 1;
    }
#endif // ..

  if (!
    gAudio__Sound__N)
    {
  const
  size_t
    gAudio__Sound__Size = 0x800;
  GAudio__SamplePCM_t *
  __gAudio__Sound__Data =
  GAlloc(
    gAudio__Sound__Size);

  for (unsigned
    n = 0,
    N = gAudio__Sound__Size;
    n < N;
  ++n)
  __gAudio__Sound__Data[n] =
    gAudio__SoundPCM->data[n %
    gAudio__SoundPCM->size];

  GAlloc__Free((void **)
   &gAudio__SoundPCM->data);
  gAudio__SoundPCM->data =
__gAudio__Sound__Data;
    }

++gAudio__Sound__N;
  return;
  }

extern
void
GAudio__Free(
  GAudio__SoundPCM_t *
  const
    gAudio__SoundPCM
  )
  {
  GAlloc__Free((void **)
   &gAudio__SoundPCM->data);
  return;
  }
