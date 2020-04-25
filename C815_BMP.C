#include <C815_STD.H>
#include "C815_BMP.H"
#include "C815_API.H"

static
inline
bool
GVideo__ValidBMPFrame(
  const
  GVideo__Frame_t *
  const
    gVideo__Frame
  )
  {
  bool
    retc;

  if (!
    gVideo__Frame->width)
bail__fail:
    GLogic__FAILURE(
     &retc);
  else
  if (
    gVideo__Frame->width
      != AlignDWRD(
    gVideo__Frame->width))
    goto // --
      bail__fail;
  else
  if (!
    gVideo__Frame->height)
    goto // --
      bail__fail;
  else
    GLogic__SUCCESS(
     &retc);

  return
    retc;
  }

extern
void
GVideo__WriteBMP__Alt(
  const
  GVideo__Palette_t *
  const
    gVideo__Palette,
  const
  void *
    gVideo__Frame__Canvas,
  const
  GVideo__Frame_t *
  const
    gVideo__Frame,

  FILE *
  const
    file__hndl
  )
  {
  if (!
    GVideo__ValidBMPFrame(
      gVideo__Frame))
  GLogic__Abort(
    "invalid frame width `%u' or height `%u'",
    gVideo__Frame->width,
    gVideo__Frame->height);

  const
  BITMAPINFOHEADER_t
    bmp__InfoHeader =
    {
   .bmp__Size = sizeof(
  BITMAPINFOHEADER_t)
 , .bmp__Width                  = gVideo__Frame->width
 , .bmp__Height                 = gVideo__Frame->height
 , .bmp__ColorPlanes            = 1
 , .bmp__BitsPerPixel           = 8
 , .bmp__CompressionMethod      = BI_RGB
 , .bmp__ImageSize              = 0
 , .bmp__Resolution__Horizontal = 300
 , .bmp__Resolution__Vertical   = 300
 , .bmp__PaletteColors          = 256
 , .bmp__ImportantColors        = 0
    };

  const
  DWRD
  __bmp__HeaderSize =
    sizeof(BITMAPFILEHEADER_t)
  + sizeof(BITMAPINFOHEADER_t)
  + sizeof(BITMAPCOLORTABLE_t[256]);
  const
  DWRD
  __bmp__BitmapSize =
    bmp__InfoHeader.bmp__Width
  * bmp__InfoHeader.bmp__Height;

  const
  BITMAPFILEHEADER_t
    bmp__FileHeader =
    {
   .bmp__Magic                  = BMP__MAGIC
 , .bmp__Size                   = __bmp__HeaderSize
                                + __bmp__BitmapSize
 , .bmp__ImageData              = __bmp__HeaderSize
    };

  BITMAPCOLORTABLE_t
    bmp__ColorTable[
  bmp__InfoHeader.bmp__PaletteColors];
  for (unsigned
    n = 0,
    N = asizof(
    bmp__ColorTable);
    n < N;
  ++n)
    {
    bmp__ColorTable[n].bmp__A = 0;
    bmp__ColorTable[n].bmp__R = gVideo__Palette[n].r;
    bmp__ColorTable[n].bmp__G = gVideo__Palette[n].g;
    bmp__ColorTable[n].bmp__B = gVideo__Palette[n].b;
    }

  FWRITE(
    bmp__FileHeader,
    file__hndl);
  FWRITE(
    bmp__InfoHeader,
    file__hndl);
  FWRITE(
    bmp__ColorTable,
    file__hndl);

  const
  uint8_t *
  const
  __bmp__BitmapTail =
 (const
  uint8_t *)
    gVideo__Frame__Canvas;

  const
  DWRD
    bmp__Height =
  bmp__InfoHeader.
    bmp__Height;
  const
  DWRD
    bmp__Width =
  bmp__InfoHeader.
    bmp__Width;

  const
  uint8_t *
  __bmp__BitmapHead =
 &__bmp__BitmapTail[
   (bmp__Height - 1)
  * bmp__Width];

  while (
  __bmp__BitmapHead
      >
  __bmp__BitmapTail)
    {
  fwrite__alt(
  __bmp__BitmapHead,
    bmp__Width,
    file__hndl);

  __bmp__BitmapHead -=
    bmp__Width;
    }
  assert(
  __bmp__BitmapHead
      ==
  __bmp__BitmapTail);
  fwrite__alt(
  __bmp__BitmapHead,
    bmp__Width,
    file__hndl);

  return;
  }

extern
void
GVideo__WriteBMP(
  const
  GVideo__Palette_t *
  const
    gVideo__Palette,
  const
  void *
    gVideo__Frame__Canvas,
  const
  GVideo__Frame_t *
  const
    gVideo__Frame,

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
GVideo__WriteBMP__Alt(
    gVideo__Palette,
    gVideo__Frame__Canvas,
    gVideo__Frame,
    file__hndl);
  return;
  }
