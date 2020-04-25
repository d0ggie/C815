#include <C815_STD.H>
#include "C815_FNT.H"
#include "C815_API.H"

// extern
const
GVideo__Font_t *
  gVideo__FontGLOBAL;

extern
GVideo__Font_t *
GVideo__FontAllocate(
  size_t
    gVideo__FontSize
  )
  {
  GVideo__Font_t *
  const
    gVideo__Font =
 (GVideo__Font_t *)
  GAlloc(
    gVideo__FontSize +
      sizeof(
    GVideo__Font_t));

  return
    gVideo__Font;
  }

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
  const
    gVideo__FrameFnt
  )
  {
  const
__GVideo__Font_t *
  const
    gVideo__FontTyped =
 &gVideo__Font->gVideo__Font[
    gVideo__FontType];

  const
  uint8_t *
  __gVideo__Font =
 &gVideo__Font->__gVideo__Font__Payload[
    gVideo__FontTyped->gVideo__Font__PayloadOffset[char__out]];

  uint8_t
 a__gVideo__FontSize[
    GVIDEO__FONTTYPE__N] =
    {
    GVIDEO__FONTSIZE__0
  , GVIDEO__FONTSIZE__1
  , GVIDEO__FONTSIZE__2
    };

  const
  uint8_t
    gVideo__FontSize =
 a__gVideo__FontSize[
    gVideo__FontType];

  const
  uint8_t
    gVideo__FontSize__Width =
  gVideo__FontSize
  , gVideo__FontSize__Height =
  gVideo__FontSize;

  const
  GVideo__Frame_t
    gVideo__Frame =
  {
    .width = gVideo__FontSize__Width
  , .height = gVideo__FontSize__Height
  };
  if (
    gVideo__FrameFnt)
  memcpy(
    gVideo__FrameFnt,
   &gVideo__Frame,
      sizeof(
    GVideo__Frame_t));

  size_t
    gVideo__Frame__CanvasSize;

  uint8_t *
  const
    gVideo__Frame__Canvas =
  GVideo__FrameAllocate(
   &gVideo__Frame,
   &gVideo__Frame__CanvasSize);

  uint8_t *
    gVideo__FrameDst__Canvas =
  gVideo__Frame__Canvas;
  const
  uint8_t *
  __gVideo__FrameDst__CanvasTail =
&gVideo__FrameDst__Canvas[
    gVideo__Frame__CanvasSize];


  uint8_t
    gVideo__FontPixel__Data = 0xff;
  uint8_t
    gVideo__FontPixel__Size = 0;

  enum
  {
    GVIDEO__FONTPIXEL__TYPE__INVISIBLE
  , GVIDEO__FONTPIXEL__TYPE__VISIBLE
  };

  typedef struct __attribute__((__packed__))
  __GVideo__FontPixel
  {
    uint8_t
      type : 1;
    uint8_t
      size : 7;
  } GVideo__FontPixel_t;

  for (;;)
    {
  const
  uint8_t
  __gVideo__FontPixel =
 *__gVideo__Font++;

  const
  GVideo__FontPixel_t *
  const
    gVideo__FontPixel =
 (GVideo__FontPixel_t *)
 &__gVideo__FontPixel;

  gVideo__FontPixel__Size
    +=
  gVideo__FontPixel->size;

  if (
    gVideo__FontPixel->type ==
    GVIDEO__FONTPIXEL__TYPE__VISIBLE)
    {
  if (!
    gVideo__FontPixel__Size)
    GLogic__Abort(
      "pixel visible, but size is `%u'",
    gVideo__FontPixel__Size);
  do
    {
   *gVideo__FrameDst__Canvas++ =
    gVideo__FontPixel__Data;
  --gVideo__FontPixel__Size;
    } while (
    gVideo__FontPixel__Size);
    }
  else
  if (
   gVideo__FontPixel__Size)
    {
    gVideo__FrameDst__Canvas
      +=
    gVideo__FontPixel__Size;
    gVideo__FontPixel__Size = 0;
    }
  else
    {
    const
    ptrdiff_t
    __gVideo__FrameOffset =
   (uintptr_t)
      gVideo__FrameDst__Canvas -
   (uintptr_t)
      gVideo__Frame__Canvas;

    const
    ptrdiff_t
    __gVideo__FontSize__Width = (
    __gVideo__FrameOffset %
      gVideo__FontSize__Width);

    if (
    __gVideo__FontSize__Width)
    GLogic__Abort(
      "pixels missing, line only had `%u' and ought to be `%u'",
    __gVideo__FontSize__Width,
      gVideo__FontSize__Width);

  if (
    gVideo__FrameDst__Canvas
      >=
  __gVideo__FrameDst__CanvasTail)
    break;
    }
    }

  return
    gVideo__Frame__Canvas;
  }

extern
void
GVideo__TextWrite(
  void *
  const
    gVideo__FrameDst__Canvas,
  const
  GVideo__Frame_t *
    gVideo__FrameDst,

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
  )
  {
  char
    text[16];
  va_list
    text__ap;
  va_start(
    text__ap,
    text__fmt);
  const
  ssize_t
  __text__N =
  vsnprintf(
    text,
      sizeof(
    text),
    text__fmt,
    text__ap);
  const
  size_t
    text__N =
  __text__N > 0 ?
  __text__N : 0;
  va_end(
    text__ap);

  GVideo__Coord_t
    gVideo__CoordDst =
    {
   .x =
    gVideo__Coord ?
    gVideo__Coord->x :
    gVideo__FrameDst->width >> 1
 , .y =
    gVideo__Coord ?
    gVideo__Coord->y : 8
    };

  for (size_t
    text__n = 0;
    text__n <
    text__N;
  ++text__n)
    {
    GVideo__Frame_t
      gVideo__FrameFnt;
    void *
      gVideo__FrameFnt__Canvas =
    GVideo__FontUnpack(
        text[
        text__n],
        gVideo__FontType,
        gVideo__Font,
       &gVideo__FrameFnt);

    GVideo__FrameCopyAlt(
      gVideo__FrameDst__Canvas,
      gVideo__FrameDst,
      gVideo__FrameFnt__Canvas,
     &gVideo__FrameFnt,
     &gVideo__CoordDst);

    gVideo__CoordDst.x += gVideo__FrameFnt.width + 2;

    GVideo__FrameFree(
      gVideo__FrameFnt__Canvas);
    }

  return;
  }
