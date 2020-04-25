#include <C815_STD.H>
#include "C815_GFX.H"
#include "C815_API.H"

// extern
GVideo__Palette_t
a__gVideo__PaletteGLOBAL__Alt[
GVIDEO__PALETTE__SIZE];
// extern
GVideo__Palette_t
a__gVideo__PaletteGLOBAL[
GVIDEO__PALETTE__SIZE];

// extern
GVideo__SpriteLookup_t
a__gVideo__SpriteLookupGLOBAL[
GVIDEO__SPRITELOOKUP__SIZE];

extern
void
GVideo__FrameDump__Dimensions(
  const
  GVideo__Frame_t *
  const
    gVideo__Frame,
  const
  char *
  const
    gVideo__FrameName
  )
  {
  printf("%.32s: width=%u, height=%u.\n",
    gVideo__FrameName ?: "<unnamed>",
    gVideo__Frame->width,
    gVideo__Frame->height);

  return;
  }

extern
GVideo__Pixel_t  *
GVideo__FrameAllocate__Alt(
  uint32_t
    width,
  uint32_t
    height
  )
  {
  const
  size_t
  __gVideo__Frame__CanvasSize =
    width *
    height;
  GVideo__Pixel_t *
    gVideo__Frame__Canvas =
  GAlloc(
  __gVideo__Frame__CanvasSize
    );
    /* // Note: Shouldn't return NULL.
  if (
    gVideo__Frame__Canvas) */
  memset(
    gVideo__Frame__Canvas, 0,
  __gVideo__Frame__CanvasSize);
  return
    gVideo__Frame__Canvas;
  }

extern
GVideo__Pixel_t *
GVideo__FrameAllocate(
  const
  GVideo__Frame_t *
  const
    gVideo__Frame,
  size_t *
    gVideo__Frame__CanvasSize
  )
  {
  const
  size_t
  __gVideo__Frame__CanvasSize =
    gVideo__Frame->width *
    gVideo__Frame->height;

  GVideo__Pixel_t *
    gVideo__Frame__Canvas =
  GAlloc(
  __gVideo__Frame__CanvasSize
    );

  memset(
    gVideo__Frame__Canvas, 0,
  __gVideo__Frame__CanvasSize);

  if (
    gVideo__Frame__CanvasSize)
   *gVideo__Frame__CanvasSize =
  __gVideo__Frame__CanvasSize;

  return
    gVideo__Frame__Canvas;
  }

extern
void
GVideo__FrameCopyAlt(
  void *
    gVideo__FrameDst__Canvas,
  const
  GVideo__Frame_t *
  const
    gVideo__FrameDst,
  const
  void *
    gVideo__FrameSrc__Canvas,
  const
  GVideo__Frame_t *
  const
    gVideo__FrameSrc,

  const
  GVideo__Coord_t *
  const
    gVideo__CoordDst)
  {
  if (
    gVideo__FrameDst->width
      <
    gVideo__FrameSrc->width)
    return;
  else
  if (
    gVideo__FrameDst->height
      <
    gVideo__FrameSrc->height)
    return;
  else
  if (
    gVideo__CoordDst->x
      >
    gVideo__FrameDst->width)
    return;
  else
  if (
    gVideo__CoordDst->y
      >
    gVideo__FrameDst->height)
    return;

  const
  void *
  const
    gVideo__FrameSrc__CanvasTail =
  PMagic__Advance(
    gVideo__FrameSrc__Canvas,
    gVideo__FrameSrc->width *
    gVideo__FrameSrc->height);
  PMagic__AdvanceAlt(
   &gVideo__FrameDst__Canvas,
    gVideo__CoordDst->y *
    gVideo__FrameDst->width);

  while (
    gVideo__FrameSrc__Canvas <
    gVideo__FrameSrc__CanvasTail)
    {
    PMagic__AdvanceAlt(
     &gVideo__FrameDst__Canvas,
      gVideo__CoordDst->x);
    memcpy(
      gVideo__FrameDst__Canvas,
      gVideo__FrameSrc__Canvas,
      gVideo__FrameSrc->width);
    PMagic__AdvanceAlt(
     &gVideo__FrameDst__Canvas,
      gVideo__FrameDst->width -
      gVideo__CoordDst->x);
    PMagic__AdvanceAlt(
     &gVideo__FrameSrc__Canvas,
      gVideo__FrameSrc->width);
    }

  return;
  }

extern
const
GVideo__Sprite_t *
GVideo__SpriteAt(
  void *
  __gVideo__SpriteData,
  GVideo__SpriteLookup_t *
    gVideo__SpriteLookup,
  unsigned
    gVideo__SpriteLookup__Indx
  )
  {
  uint8_t *
    gVideo__SpriteData =
 (uint8_t *)
  __gVideo__SpriteData;
  gVideo__SpriteData
    +=
  gVideo__SpriteLookup[
  gVideo__SpriteLookup__Indx].
  gVideo__SpriteOffset;

  return
   (GVideo__Sprite_t *)
  gVideo__SpriteData;
  }

extern
void
GVideo__FrameDump(
  const
  GVideo__Pixel_t *
    gVideo__Frame__Canvas,
  size_t
    gVideo__Frame__CanvasSize,
  const
  GVideo__Palette_t *
  const
    gVideo__Palette,
  unsigned
    gVideo__Frame__Indx
  )
  {
  char
    name[80];
  snprintf(
    name,
      sizeof(
    name),
    "DUMP\\FRMOBJ%02X.RAW",
    gVideo__Frame__Indx);

  FILE * __attribute__((__cleanup__(GStdIO__Close)))
    hndl__frame =
  fopen(
    name, "wb");
  if (!!
    hndl__frame)
  for (unsigned
    n = 0,
    N = gVideo__Frame__CanvasSize;
    n < N;
  ++n)
  fwrite__alt(
    gVideo__Palette[gVideo__Frame__Canvas[n]].__rgb, 3,
    hndl__frame);

  return;
  }

extern
void
GVideo__SpriteUnpack(
  GVideo__Pixel_t *
  __gVideo__FrameDst,
  const
  GVideo__Sprite_t *
  const
    gVideo__Sprite
  )
  {
  const
  GVideo__Pixel_t *
  __gVideo__FrameSrc =
  gVideo__Sprite->frame__src;
    /* // Not: Unused for now.
  const
  GVideo__Pixel_t *
  const
    gVideo__FrameSrc__Base =
  gVideo__Sprite->frame__src */
    ;

  typedef union
__GVideoFrame__Pointer
    {
  uint8_t *
  __u8;
  uint16_t *
  __u16;
  uint32_t *
  __u32;
    }
  GVideoFrame__Pointer_t;
  typedef union
__GVideoFrame__Pointer__const
    {
  const
  uint8_t *
  __u8;
  const
  uint16_t *
  __u16;
  const
  uint32_t *
  __u32;
    }
  GVideoFrame__Pointer__const_t;

  GVideoFrame__Pointer_t
    gVideo__FrameDst =
      {
  __gVideo__FrameDst };
  GVideoFrame__Pointer__const_t
    gVideo__FrameSrc =
      {
  __gVideo__FrameSrc };

  const
  GVideo__Pixel_t *
  const
    gVideo__FrameDst__Base =
__gVideo__FrameDst;

  uint16_t
    gVideo__FrameHeight =
  gVideo__Sprite->height;

  for (;;)
    {
    GVideo__Pixel_t
      gVideo__FramePixelCommand =
   *gVideo__FrameSrc.__u8++;

  enum
  {
    GVIDEO__FRAMEPIXELCOMMAND__SKIP_N = 0xff
  , GVIDEO__FRAMEPIXELCOMMAND__SKIP_L = 0xfe
  };

    if (
      gVideo__FramePixelCommand ==
      GVIDEO__FRAMEPIXELCOMMAND__SKIP_N)
      {
    gVideo__FrameDst.__u8 +=
   *gVideo__FrameSrc.__u8++;
      }
    else
    if (
      gVideo__FramePixelCommand ==
      GVIDEO__FRAMEPIXELCOMMAND__SKIP_L)
      {
    if (!
    --gVideo__FrameHeight)
      break;

    gVideo__FrameDst.__u8
      +=
    gVideo__Sprite->width - (
     (gVideo__FrameDst.__u8 - gVideo__FrameDst__Base) %
    gVideo__Sprite->width);
      }
    else
      {
    if (
      gVideo__FramePixelCommand & 1)
     *gVideo__FrameDst.__u8++ =
     *gVideo__FrameSrc.__u8++;
    gVideo__FramePixelCommand >>= 1;

    if (
      gVideo__FramePixelCommand & 1)
     *gVideo__FrameDst.__u16++ =
     *gVideo__FrameSrc.__u16++;
    gVideo__FramePixelCommand >>= 1;

    while (
      gVideo__FramePixelCommand--)
     *gVideo__FrameDst.__u32++ =
     *gVideo__FrameSrc.__u32++;
      }
    }

  return;
  }

extern
void
GVideo__SpriteDump__Dimensions(
  const
  GVideo__Sprite_t *
  const
    gVideo__Sprite
  )
  {
  printf("width=%u, height=%u\n",
    gVideo__Sprite->width,
    gVideo__Sprite->height);
  return;
  }

extern
void *
GVideo__SpriteUnpackAndAllocateFrame(
  const
  GVideo__Sprite_t *
  const
    gVideo__Sprite
  )
  {
  void *
    gVideo__Frame__Canvas =
  GVideo__FrameAllocate__Alt(
    gVideo__Sprite->width,
    gVideo__Sprite->height);
  if (
    gVideo__Frame__Canvas)
  GVideo__SpriteUnpack(
    gVideo__Frame__Canvas,
    gVideo__Sprite);
  return
    gVideo__Frame__Canvas;
  }

extern
void
GVideo__SpriteDump(
  const
  GVideo__Sprite_t *
  const
    gVideo__Sprite,
  unsigned
    gVideo__Sprite__Indx
  )
  {
  GVideo__SpriteDump__Dimensions(
    gVideo__Sprite);
    /*
  for (unsigned n = 0; n < 128; ++n)
  printf("%02x: %08x (%4u)\n", n,
    gVideo__Sprite->frame__key[n],
    gVideo__Sprite->frame__key[n]); */

  const
  GVideo__Frame_t
    gVideo__Frame =
  {
   .width =
    gVideo__Sprite->
    width
 , .height =
    gVideo__Sprite->
    height
  };

  size_t
    gVideo__Frame__CanvasSize;
  GVideo__Pixel_t *
    gVideo__Frame__Canvas =
  GVideo__FrameAllocate(
   &gVideo__Frame,
   &gVideo__Frame__CanvasSize);

  GVideo__SpriteUnpack(
    gVideo__Frame__Canvas,
    gVideo__Sprite);
  GVideo__FrameDump(
    gVideo__Frame__Canvas,
    gVideo__Frame__CanvasSize,
 a__gVideo__PaletteGLOBAL,
    gVideo__Sprite__Indx);

  GVideo__FrameFree(
    gVideo__Frame__Canvas);

  return;
  }