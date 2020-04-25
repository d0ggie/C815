#ifndef __C815_BMP_H
#define __C815_BMP_H

#include "C815_INT.H"
#include "C815_GFX.H"

typedef struct __attribute__((__packed__))
__BITMAPFILEHEADER
{
#define \
  BMP__MAGIC 0x4d42
  WORD bmp__Magic;
  DWRD bmp__Size;
  WORD bmp__Reserved[2];
  DWRD bmp__ImageData;
} BITMAPFILEHEADER_t;

static_assert(sizeof(
  BITMAPFILEHEADER_t) == 14);

typedef struct __attribute__((__packed__))
__BITMAPINFOHEADER
{
  DWRD bmp__Size;
  DWRD bmp__Width;
  DWRD bmp__Height;
  WORD bmp__ColorPlanes;
  WORD bmp__BitsPerPixel;
  DWRD bmp__CompressionMethod;
  DWRD bmp__ImageSize;
  DWRD bmp__Resolution__Horizontal;
  DWRD bmp__Resolution__Vertical;
  DWRD bmp__PaletteColors;
  DWRD bmp__ImportantColors;
} BITMAPINFOHEADER_t;

static_assert(sizeof(
  BITMAPINFOHEADER_t) == 40);

typedef struct __attribute__((__packed__))
__BITMAPCOLORRABLE
{
  union
  {
  struct
  {
    // Note:  These are stored in reverse order.  Many source describe these as
    // `RGB', but note in appendixes that the order is actually reversed.
  BYTE bmp__B;
  BYTE bmp__G;
  BYTE bmp__R;

    // Note: `Alpha' channel is likely ignored by all readers.
  BYTE bmp__A;
  };
  BYTE bmp__BGRA[4];
  };
} BITMAPCOLORTABLE_t;

enum
__BITMAPCOMPRESSIONMETHOD
{
  BI_RGB = 0
} BITMAPCOMPRESSIONMETHOD_t;

static
inline
DWRD
AlignDWRD(
  DWRD
    n
  )
  {
  const
  DWRD
  __n =
  n %
    sizeof(
  DWRD);
  if (
  __n)
    n +=
    sizeof(
  DWRD) -
  __n;

  return
    n;
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
  );

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
  );

#endif//__C815_BMP_H
