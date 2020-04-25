#ifndef __C815_FNT_H
#define __C815_FNT_H

#include "C815_GFX.H"

// Note:  The number of font type faces is hard-coded into the game code.  Each
// type face  only has one  fixed size,  which is also hardcoded.  It  would be
// possible to automatically determine the font size, though..
typedef enum
__GVideo__FontType
{
  GVIDEO__FONTTYPE__0
, GVIDEO__FONTTYPE__1
, GVIDEO__FONTTYPE__2

, GVIDEO__FONTTYPE__N
} GVideo__FontType_t;

typedef enum
__GVideo__FontSize
{
  GVIDEO__FONTSIZE__0 = 0x20
, GVIDEO__FONTSIZE__1 = 0x10
, GVIDEO__FONTSIZE__2 = 0x08
} GVideo__FontSize_t;

static
inline
size_t
GVideo__FontSize(
  GVideo__FontSize_t
    gVideo__FontSize
  )
  {
  static
  GVideo__FontSize_t
 a__gVideo__FontSize[
    GVIDEO__FONTTYPE__N] =
  {
    GVIDEO__FONTSIZE__0
  , GVIDEO__FONTSIZE__1
  , GVIDEO__FONTSIZE__2
  };
  assert(
    gVideo__FontSize <
      asizof(
 a__gVideo__FontSize));
  return
 a__gVideo__FontSize[
    gVideo__FontSize];
  }

// Note: Each type face has 256 characters.  This is hard coded, too.
#define \
  GVIDEO__FONT__CHARACTERS 256

typedef uint32_t
  GVideo__FontOffset_t;

typedef struct
__GVideo__Font
{
// Note: Each  type face has  an offset table which  points to  the RLE encoded
// payload data.  Offsets are complete,  so it's not necessary to know how many
// other type faces there would be.
  GVideo__FontOffset_t
    gVideo__Font__PayloadOffset[
  GVIDEO__FONT__CHARACTERS];
} __GVideo__Font_t;
typedef struct
__GVideo__Font_t
{
// Note:  Offset tables and RLE encoded payload data lives in a single  `BLOB',
// inside which  all offsets must  point.  As stated above,  the number of type
// faces, i.e. fonts, is hard coded.
  union
    {
__GVideo__Font_t
    gVideo__Font[
  GVIDEO__FONTTYPE__N];
  const
  uint8_t
  __gVideo__Font__Payload[0];
    };
} GVideo__Font_t;

extern
const
GVideo__Font_t *
  gVideo__FontGLOBAL;

extern
GVideo__Font_t *
GVideo__FontAllocate(
  size_t
    gVideo__FontSize
  );

extern
void *
GVideo__FontUnpack(
  unsigned
  char
    char__out,
  GVideo__FontType_t
    gVideo__FontType,
  const
  GVideo__Font_t *
  const
    gVideo__Font,
  GVideo__Frame_t *
    gVideo__FrameFnt
  );

extern
void
GVideo__TextWrite(
  void *
  const
    gVideo__Frame__Canvas,
  const
  GVideo__Frame_t *
  const
    gVideo__Frame,

  const
  GVideo__Font_t *
  const
    gVideo__Font,
  GVideo__FontType_t
    gVideo__FontType,

  const
  GVideo__Coord_t *
  const
    gVideo__Coord,

  const
  char *
  const
    text__fmt, ...
  );

#endif//__C815_FNT_H
