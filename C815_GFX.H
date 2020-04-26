#ifndef __C815_GFX_H
#define __C815_GFX_H

typedef union __attribute__((__transparent_union__))
__void__p1
{
  void *
  __void;
  const
  void *
  __void__const;
} void__p1_t;

typedef union __attribute__((__transparent_union__))
__void__p2
{
  void **
  __void;
  const
  void **
  __void__const;
} void__p2_t;

static
inline
void *
PMagic__Advance(
  void__p1_t
    void__ptr,
  size_t
    N
  )
  {
    // Note: Advances given pointer and returns the modified value.
  uintptr_t
  __void__ptr =
 (uintptr_t)
    void__ptr.__void;
__void__ptr += N;
  return (void *)
  __void__ptr;
  }

static
inline
void
PMagic__AdvanceAlt(
  void__p2_t
    void__ptr,
  size_t
    N
  )
  {
    // Note: Advances in place.
  if (!!
    void__ptr.__void)
    {
   *void__ptr.__void =
PMagic__Advance(
   *void__ptr.__void, N);
    }

  return;
  }

typedef uint8_t
  GVideo__Pixel_t;

typedef struct __attribute__((__packed__))
__GVideo__Palette
{
  union
    {
  struct
    {
  GVideo__Pixel_t
    r
  , g
  , b
    ;
    };
// Note:  While the palette is  indeed  stored and transferred  to hardware  in
// in this very same 8-bit format,  only 6-bits of the palette is actually used
// to render color information.  This is a limitation of the VGA era.
  GVideo__Pixel_t
  __rgb[3];
    };
} GVideo__Palette_t;

#define \
GVIDEO__PALETTE__SIZE                                                          \
   (sizeof(                                                                    \
  GVideo__Pixel_t) << 8)
// Note: This alternative palette doesn't appear to be used.
extern
GVideo__Palette_t
a__gVideo__PaletteGLOBAL__Alt[
GVIDEO__PALETTE__SIZE];
extern
GVideo__Palette_t
a__gVideo__PaletteGLOBAL[
GVIDEO__PALETTE__SIZE];

typedef struct __attribute__((__packed__))
__GVideo__SpriteLookup
{
  uint32_t
    gVideo__SpriteOffset;
} GVideo__SpriteLookup_t;

#define \
GVIDEO__SPRITELOOKUP__SIZE                                                     \
  0x0100
#define \
GVIDEO__SPRITEKEYTBL__SIZE                                                     \
  0x0080

extern
GVideo__SpriteLookup_t
a__gVideo__SpriteLookupGLOBAL[
GVIDEO__SPRITELOOKUP__SIZE];

typedef struct __attribute__((__packed__))
__GVideo__Sprite
{
  uint16_t
    width
  , height;
// Note: Sprite data includes a fixed key index.  Each index  always points  to
// some RLE node.  Traversing this index is not necessary for unpacking the RLE
// encoding.  The order of the index is  hot-swapped  after  the data has  been
// loaded and un-obfuscated.  Apart from that,  the game doesn't appear to make
// use of this data in any way.
  uint32_t
    frame__key[
  GVIDEO__SPRITEKEYTBL__SIZE];
// Note: RLE encoded payload.  To skip evaluating  the entire data  stream  the
// above index can be used to seek to each new RLE node.
  uint8_t
    frame__src[];
} GVideo__Sprite_t;

static
inline
size_t
GVideo__SpriteSize(
  const
  GVideo__Sprite_t *
  const
    gVideo__Sprite
  )
  {
  return
    gVideo__Sprite->width *
    gVideo__Sprite->height
    ;
  }

#define \
  GVIDEO__SPRITES                                                              \
  0x001d

typedef struct
__GVideo__Frame
{
  uint32_t
    width
  , height
    ;
} GVideo__Frame_t;

static
inline
size_t
GVideo__FrameSize(
  const
  GVideo__Frame_t *
  const
    gVideo__Frame
  )
  {
  return
    gVideo__Frame->width *
    gVideo__Frame->height
    ;
  }

static
inline
bool
GVideo__Frame__Equal(
  const
  GVideo__Frame_t *
  const
    gVideo__Frame__0,
  const
  GVideo__Frame_t *
  const
    gVideo__Frame__1
  )
  {
  return memcmp(
    gVideo__Frame__0,
    gVideo__Frame__1,
      sizeof(
    GVideo__Frame_t)) ==0;
  }

enum
{
  GVIDEO__PIXELPCX__REPEAT = 0b11
};

// Note:  The game play^Wbattlefield is encoded using  a RLE encoding.  This is
// exactly  the same format used  by  PCX,   PiCture eXchange,   format.  Image
// dimensions  are  hard coded  and there is  no header of any kind.  Also,  no
// other images are stored like this.
typedef struct __attribute__((__packed__))
__GVideo__PixelPCX
{
  union
    {
  struct
  {
  GVideo__Pixel_t
    data__repeat : 6;
// Note: If mode is `repeat' (i.e. two topmost bit set),  then the repeat count
// is  encoded in the lower bits and another pixel (byte) shall be read,  which
// is then repeated.  Otherwise, this is a plain pixel.
  GVideo__Pixel_t
    mode : 2;
  };
  GVideo__Pixel_t
    data;
    };
} GVideo__PixelPCX_t;
static_assert(sizeof(
  GVideo__PixelPCX_t) == 1);

typedef struct
__GVideo__Coord
{
  uint32_t
    x
  , y
    ;
} GVideo__Coord_t;

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
  );

extern
GVideo__Pixel_t *
GVideo__FrameAllocate__Alt(
  uint32_t
    width,
  uint32_t
    height
  );

static
inline
void
GVideo__FrameFree(
  void *
    gVideo__Frame__Canvas
  )
  {
  if (!!
    gVideo__Frame__Canvas)
  free(
    gVideo__Frame__Canvas);
  return;
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
  );

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
    gVideo__CoordDst
  );

static
inline
void
GVideo__FrameCopy(
  GVideo__Pixel_t *
    gVideo__FrameDst__Canvas,
  const
  GVideo__Frame_t *
  const
    gVideo__FrameDst,
  const
  GVideo__Pixel_t *
    gVideo__FrameSrc__Canvas,
  const
  GVideo__Frame_t *
  const
    gVideo__FrameSrc
  )
  {
  const
  GVideo__Coord_t
    gVideo__CoordDst = {
   .x = 0
 , .y = 0
    };
GVideo__FrameCopyAlt(
    gVideo__FrameDst__Canvas,
    gVideo__FrameDst,
    gVideo__FrameSrc__Canvas,
    gVideo__FrameSrc,
   &gVideo__CoordDst);

  return;
  }

static
inline
void
GVideo__FrameCopy__Center(
  GVideo__Pixel_t *
    gVideo__FrameDst__Canvas,
  const
  GVideo__Frame_t *
  const
    gVideo__FrameDst,
  const
  GVideo__Pixel_t *
    gVideo__FrameSrc__Canvas,
  const
  GVideo__Frame_t *
  const
    gVideo__FrameSrc
  )
  {
  assert(
    gVideo__FrameDst->width
      >=
    gVideo__FrameSrc->width);
  assert(
    gVideo__FrameSrc->height
      >=
    gVideo__FrameSrc->height);
  const
  GVideo__Coord_t
    gVideo__CoordDst = {
   .x = (
    gVideo__FrameDst->width -
    gVideo__FrameSrc->width)
      >> 1
 , .y = (
    gVideo__FrameDst->height -
    gVideo__FrameSrc->height)
      >> 1
    };
GVideo__FrameCopyAlt(
    gVideo__FrameDst__Canvas,
    gVideo__FrameDst,
    gVideo__FrameSrc__Canvas,
    gVideo__FrameSrc,
   &gVideo__CoordDst);

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
  );

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
  );

extern
void
GVideo__SpriteUnpack(
  GVideo__Pixel_t *
  __gVideo__FrameDst,
  const
  GVideo__Sprite_t *
  const
    gVideo__Sprite
  );

extern
void
GVideo__SpriteDump__Dimensions(
  const
  GVideo__Sprite_t *
  const
    gVideo__Sprite
  );

extern
void *
GVideo__SpriteUnpackAndAllocateFrame(
  const
  GVideo__Sprite_t *
  const
    gVideo__Sprite
  );

extern
void
GVideo__SpriteDump(
  const
  GVideo__Sprite_t *
  const
    gVideo__Sprite,
  unsigned
    gVideo__Sprite__Indx
  );

#endif//__C815_GFX_H
