#include <C815_STD.H>

#include "C815.H"
#include "C815_GFX.H"
#include "C815_SFX.H"
#include "C815_FNT.H"
#include "C815_API.H"
#include "C815_BMP.H"
#include "C815_WAV.H"

// ============================================================================
//   C815 -- AN UNPACKER FOR ``RYSSÄN KAUHU''
// ----------------------------------------------------------------------------
// Note: ``Ryssän Kauhu'  is a small Finnish game released in  1998.  The  game
// works well in DOSBox, so simply give it a try.
//
// The game runs in 486 protected mode,  and has  a standard 16-bit stub loader
// which passes the execution to DOS4/GW, which ultimately loads the LE (Linear
// Executable) objects and applies relocation information.  Without  this reloc
// information patched it is useless  to reverse engineer this game,  as  there
// are about  one thousand patched  memory locations.  The game  is  apparently
// written using WATCOM C/C++ compiler from mid 1990s;  To be exact,  copyright
// message dates it to 1994.
//
// The  standard C library portions  can be  cross referenced using  OpenWATCOM
// sources  (the earliest version is recommended),  and there are  no huge code
// differences  apart  from  clock initialization   (this  version  does  delay
// compensation while the OpenWATCOM is simplified) and  `_nmalloc'  is a  more
// archaic revision.  The game also uses it's own block allocation routine,  in
// which it allocates up to 256K blocks,  injects block information  and  spits
// out smaller  data blocks.  The allocation code,  while quite brief,  is  not
// fully studied as it's not a hugely interesting.
// The game code has  many unused  code blocks left, the machine code has many,
// many strange quirks and no data is de-duplicated.  As these unused functions
// are seemingly never called, those very not studied extensively.  It would be
// interesting to know, if the game was built using a pre-existing game library
// which wasn't then stripped.  Also, the game code is not very abstract.  E.g.
// sound code is duplicated, so instead of reading data and then perhaps moving
// in on a dedicated audio DRAM if the target device supports it,  the  reading
// code is simply duplicated and with any required special processing.
//
// When the game is started,  it tries to probe sound device and then loads any
// assets that stored in `GRAPH.DAT'.  Unlike the name suggests, it also stores
// all sound assets.  After the loading is done, any requires precalculation is
// done.  The game logic runs in a loop,  which processes a double linked  list
// of enemy soldiers.  The list member data contains information such  as state
// and location.  Mouse position is queried in this very same loop,  as is done
// all necessary  frame buffer copying and updating.  Basically  no element  is
// updated  unless  necessary,  e.g.  remaining  ammo count  is compared  to  a
// previously rendered value:  Unless it has changed, no frame buffer update is
// done.  Basically everything is hard coded,  which makes  reverse engeneering
// effors easy to some extent.  If the game requests sprite at index `0x1c', it
// is a crosshair.  Therefore, using this information is is possbile to fill in
// the blanks about the referenced variables.
//
// Assets, which the game uses, are listed below.  What sparked this project is
// the fact that all assets are obfuscated  and  the most of the assets are RLE
// encoded.  The latter fact was obviously revealed much later.  A simple `XOR'
// obfuscation or encryption is used,  and the key is different for each asset.
// There is,  for some reason,  a single exception:  That  is  the sound effect
// data.  It is not obfuscated in any way,  except  that the first sound sample
// will have some processing  and  looping applied to it  (and that  code  also
// involves a memory leak, as the original sample is not released).
//
//   - Sound effect data:
// Procesing starts at a fixed location,  and the code reads  a single  element
// header which describes the length of the data to be read.  If sounds are not
// enabled (neither Soundblaster or Gravis Ultrasound was detected) the data is
// skipped simply by doing a `fseek'.  The both devices use  the same PCM data,
// but depending on the used device it  is handle differently.  On Soundblaster
// data is transferred from local memory, while on GUS it is transferred on the
// card.  Unlike other assets,  the number of sound effects is stored onto data
// file.  It is not used for anything else than reading,  though,  and the game
// still plays sounds based on fixed indexes -- nothing even checks that enough
// sound effects have been actually read.
//
// Sound data  is  stored  on  the  file   as  signed  PCM samples.  This  will
// transformed by the game during loading process as  unsiged PCM.  As  both of
// supported sound cards use only unsigned PCM sample, why this wasn't was done
// when  the data file created only  the makers  of this game know.  Also,  the
// first sound sample is looped / synthesized (the machine gun sound).  As  the
// generated data is still very small,  it is also a mystery why such steps are
// not simply generated once and then stored.
//
//   - Palette data:
// Stored at fixed locations.  Two palettes are read, one of which is not used.
// There's an unused function,  which would apply that palette.  All  graphical
// assets use the same palette,  which appears to be a very standard one.  None
// of  the indexed colors  are  used as  an alpha mask.  Instead,  transparency
// information is `embedded' within the RLE encoding as special value.
//
//   - Graphics data:
// Only a single asset, the main battlefield.  Stored at a fixed location,  but
// the data is RLE encoded.  RLE encoding is the same used in PCX files, so, an
// expected choice.
//
//   - Font data:
// Three distinct fonts.  Each font has  a lookup table which points  to  a RLE
// encoded data for that particular character.  The number of fonts and size of
// font (only fixed width characters) is hard coded.  Interestingly,  each font
// has all 256 `characters' populated.
//
//   - Sprite data:
// Sprites include aiming crosshair,  hit marker and enemy soldiers.  Animation
// of (enemy)  sprites is done by incrementing  a counter  and  adding  a fixed
// number to that based on the enemy state (e.g. alive, shooting or dying).
//
// Stored at fixed location.  Each sprite has  a header  and  RLE encoded data.
// Sprites  are referenced using  a lookup table,  which points  to  the sprite
// header.  No idea if  RLE encoding method  is  borrowed  from  somwhere.  The
// lookup table is of  a  fixed size.  It  has only  a fixed number  of  `real'
// offsets and the rest are just nonce.  The number of sprites is hard coded.
// The game has two different functions to render sprites:  One  does  unscaled
// output while the other does scaling.  The scaling is done by pre-calculating
// multiplication and/or shifting tables.  While  the unscaling version  simply
// passes the decoded data to a frame buffer,  the  scaling version passes  the
// raw pixel  (incl. `transparency',  i.e.  a background  framebuffer  is  used
// instead of an encoded pixel value) to a corresponding scaling table first.
// The sprite header includes  an offset table which points to each pixel node,
// i.e. RLE encoded data.  No  advantage is taken  by using  this data.   Also,
// while  `huge' (64K)  data tables are allocated  to speed up calculations the
// RLE encoding is always done `on-the-fly' and is never cached -- no idea why,
// as the sprites are relative small.
//
//   - Random data:
// The game uses a standard C-library `rand' for `AI'.  However,  depending  on
// the operation  the game assets include  two data tables that are used  as  a
// seed value (given that certain conditions are met).  Also, one table is read
// and used as an input for an another random offset table.  This  might  be an
// unfinished feature,  as after the calculations no non-zero data is produced.
// That zero data is still used by the game,  naturally  not having any  effect
// whatsoever.
//
//   - Level data:
// Stored  inside  the  executable image.  This  includes  (but not limited to)
// information such  as number of enemies and ammuniton  in  each level.  Also,
// unique initial seed values are stored in this table of data.  The very first
// (zeroth) is hidden, as level counter begins from one, but the level array it
// references is of course zero based.

static
const
char *
const
__file__default =
  "GRAPH.DAT";

static
const
char *
  file;


static
uint32_t
GGraph__LoadSpriteData__Size(
  FILE *
  const
    file__hndl
  )
  {
  uint32_t
    gVideo__SpriteData__Size;
  FREAD(
    gVideo__SpriteData__Size, file__hndl);
  gVideo__SpriteData__Size
    ^=
  C815__MAGIC__1;
  printf("SPRSIZ: %08x.\n",
    gVideo__SpriteData__Size);

  return
    gVideo__SpriteData__Size;
  }

static
void
GGraph__LoadSpriteLookup(
  FILE *
  const
    file__hndl
  )
  {
  FREAD(
 a__gVideo__SpriteLookupGLOBAL,
    file__hndl);
  for (unsigned n = 0; n < asizof(
 a__gVideo__SpriteLookupGLOBAL); ++n)
    {
    const
    uint32_t
      c815__magic__3 =
n - C815__MAGIC__3;
 a__gVideo__SpriteLookupGLOBAL[n].gVideo__SpriteOffset
      ^=
    c815__magic__3;
    }
  FDUMP(
 a__gVideo__SpriteLookupGLOBAL);

  return;
  }

static
void
GGraph__LoadSound(
  FILE *
  const
    file__hndl
  )
  {
  uint32_t
    gAudio__Sound__N;
  FREAD(
    gAudio__Sound__N,
    file__hndl);

  FTELL(
    file__hndl);
  printf("SNDOBJ: %02x.\n",
    gAudio__Sound__N);

  for (unsigned
    n = 0,
    N = gAudio__Sound__N;
    n < N;
  ++n)
    {
    GAudio__SoundPCM_t
      gAudio__SoundPCM;
    GAudio__LoadSound(
     &gAudio__SoundPCM,
      file__hndl);
      /* // Note: Not enabled.
    FTELL(
      file__hndl) */
        ;
    printf("SNDOBJ, %02x: size %04x.\n", n,
      gAudio__SoundPCM.size);

#ifdef _DEBUG
    GAudio__DumpSound(
     &gAudio__SoundPCM, n)
#endif // ..
      ;
    GAudio__WriteWAV(
     &gAudio__SoundPCM,
      "DUMP\\SNDOBJ%02U.WAV", n);

    GAudio__Free(
     &gAudio__SoundPCM);
    }

  return;
  }

static
void
GGraph__LoadFont(
  FILE *
  const
    file__hndl
  )
  {
  uint32_t
    gVideo__FontSize;
  FREAD(
    gVideo__FontSize, file__hndl);
  printf("FNTSIZ: %08x.\n",
    gVideo__FontSize);
  uint8_t *
  const
  __gVideo__Font =
 (uint8_t *)
  GVideo__FontAllocate(
    gVideo__FontSize);
  GVideo__FontOffset_t *
    gVideo__FontLookup =
 (GVideo__FontOffset_t *)
  __gVideo__Font;
  const
  size_t
    gVideo__FontLookup__Size =
  GVIDEO__FONTTYPE__N *
  GVIDEO__FONT__CHARACTERS *
    sizeof(
  GVideo__FontOffset_t);

  uint8_t *
    gVideo__Font =
  __gVideo__Font +
    gVideo__FontLookup__Size;
  fread__alt(
    gVideo__FontLookup,
    gVideo__FontLookup__Size, file__hndl);
  fread__alt(
    gVideo__Font,
    gVideo__FontSize, file__hndl);

  for (unsigned
    n = 0,
    N = GVIDEO__FONTTYPE__N * GVIDEO__FONT__CHARACTERS;
    n < N;
  ++n)
    gVideo__FontLookup[n]
      ^=
    C815__MAGIC__4;

  FDUMP__ALT(
  __gVideo__Font,
    GVIDEO__FONTTYPE__N *
    GVIDEO__FONT__CHARACTERS +
    gVideo__FontSize);

  gVideo__FontGLOBAL =
 (const
  GVideo__Font_t *)
__gVideo__Font;

  FDUMP__ALT(
    gVideo__FontLookup,
    gVideo__FontLookup__Size);
  FDUMP__ALT(
    gVideo__Font,
    gVideo__FontSize);

  return;
  }

static
void
GGraph__LoadPalette(
  FILE *
  const
    file__hndl
  )
  {
  FREAD(
 a__gVideo__PaletteGLOBAL,
    file__hndl);
  static_assert(sizeof(
 a__gVideo__PaletteGLOBAL)
    == 0x0300);

  for (unsigned
    n = 0x0000,
    N = 0x0100;
    n < N;
  ++n)
  for (unsigned
    m = 0x0000,
    M = 0x0003;
    m < M;
  ++m)
    {
    const
    unsigned
    __n = n * 3 + m;
 a__gVideo__PaletteGLOBAL[n].__rgb[m]
      ^=
    C815__MAGIC__5(__n);
    }

    // Note: VGA-palette is mere 6-bits per color.  This is obviously not  done
    // by the game.
  for (unsigned
    n = 0x0000,
    N = 0x0100;
    n < N;
  ++n)
  for (unsigned
    m = 0x0000,
    M = 0x0003;
    m < M;
  ++m)
    {
 a__gVideo__PaletteGLOBAL[n].__rgb[m]
       <<= 2;
 a__gVideo__PaletteGLOBAL[n].__rgb[m]
        |= 3;
    }

  FDUMP(
 a__gVideo__PaletteGLOBAL);
  return;
  }

static
GVideo__Pixel_t *
GGraph__LoadPCX(
  const
  GVideo__Frame_t *
  const
    gVideo__Frame,

  FILE *
  const
    file__hndl
  )
  {
  size_t
    gVideo__Frame__CanvasSize;

  GVideo__Pixel_t *
  const
    gVideo__Frame__Canvas =
  GVideo__FrameAllocate(
    gVideo__Frame,
   &gVideo__Frame__CanvasSize);
  assert(
    gVideo__Frame__CanvasSize
      <= 0xfa00);

  GVideo__Pixel_t *
  __gVideo__Frame__Canvas =
  gVideo__Frame__Canvas;

  for (unsigned
    n = 0x0000,
    N = gVideo__Frame__CanvasSize;
    n < N;
    )
    {
    const
    uint8_t
      c815__magic__6 =
    C815__MAGIC__6(n);

    GVideo__PixelPCX_t
      gVideo__PixelPCX;
    FREAD(
      gVideo__PixelPCX,
      file__hndl);
    gVideo__PixelPCX.data
      ^=
    c815__magic__6;

    if (
      gVideo__PixelPCX.mode
        ==
      GVIDEO__PIXELPCX__REPEAT)
      {
    GVideo__Pixel_t
      gVideo__Pixel;
    FREAD(
      gVideo__Pixel,
      file__hndl);
    gVideo__Pixel
      ^=
    c815__magic__6;

    for (unsigned
      n__repeat = 0,
      N__repeat = gVideo__PixelPCX.data__repeat;
      n__repeat <
      N__repeat;
    ++n,
    ++n__repeat)
   *__gVideo__Frame__Canvas++ =
      gVideo__Pixel;
      }
    else
      {
   *__gVideo__Frame__Canvas++ =
      gVideo__PixelPCX.data;
    ++n;
      }
    }

  return
    gVideo__Frame__Canvas;
  }


// ============================================================================
//   MAIN
// ----------------------------------------------------------------------------
int
main(
  int
    argc,
  char **
    argv)
  {
  if (
    argc > 1)
    {
    file = argv[1];
    printf(
      "Using `%.128s' instead of the default `%.128s'.\n",
      file,
    __file__default);
    }
  else
    file =
  __file__default;

  FILE * __attribute__((__cleanup__(GStdIO__Close)))
    file__hndl =
  fopen__alt(
    file, "rb");

  FREAD(
 a__gVideo__PaletteGLOBAL__Alt, file__hndl);

  const
  uint32_t
    gVideo__SpriteData__Size =
  GGraph__LoadSpriteData__Size(
    file__hndl);

    // Note: This  doesn't  appear  to be used anywhere.  All  these  of  these
    // allocated sizes are copied verbatim from the game code.
  uint16_t * __attribute__((__cleanup__(GAlloc__FreeUI16)))
    gLogic__Buffer__0 =
  GAlloZ(0x8000 * sizeof(uint16_t));

    // Note:  his is used as a seed for C-library  `rand' when doing enemy `AI'
    // processing.
  uint16_t * __attribute__((__cleanup__(GAlloc__FreeUI16)))
    gLogic__Buffer__1 =

    // Note: This is used as input to calculate  MG and aim  horizontal  random
    // position.  Does not appear to yield anything.
  GAlloZ(0x8000 * sizeof(uint16_t));
   int16_t * __attribute__((__cleanup__(GAlloc__FreeI16)))
    gVideo__Buffer__2 =
  GAlloZ(0x8000 * sizeof( int16_t));
    // Note: See above.  Also, this is a statically allocated table in the game
    // code.
  uint16_t * __attribute__((__cleanup__(GAlloc__FreeUI16)))
    gVideo__Buffer__3 =
  GAlloZ(0x4000 * sizeof(uint16_t));

    // Note: This is used as a seed fo r C-library `rand' when doing player and
    // enemy sounds.
  uint16_t * __attribute__((__cleanup__(GAlloc__FreeUI16)))
    gLogic__Buffer__2 =
  GAlloZ(0x8000 * sizeof(uint16_t));

  fread__alt(
    gLogic__Buffer__0, 0x1000 * sizeof(uint16_t),
    file__hndl);
  fread__alt(
    gLogic__Buffer__1, 0x1000 * sizeof(uint16_t),
    file__hndl);
  fread__alt(
    gVideo__Buffer__2, 0x1000 * sizeof( int16_t),
    file__hndl);
  fread__alt(
    gLogic__Buffer__2, 0x1000 * sizeof(uint16_t),
    file__hndl);

  for (unsigned
    n = 0x0000,
    N = 0x1000;
    n < N;
  ++n)
    {
    const
    uint16_t
      c815__magic__2 =
    C815__MAGIC__2 - (n << 1);
    gLogic__Buffer__0[n]
      ^=
    C815__MAGIC__2;
    gLogic__Buffer__1[n]
      ^=
    c815__magic__2;
    gVideo__Buffer__2[n]
      ^=
    c815__magic__2;
    gLogic__Buffer__2[n]
      ^=
    c815__magic__2;
    }

/* // Note: `ebx' is `n'.
00011E37 loc_11E37:
00011E37                 cmp     ebx, 4000h      ; Compare Two Operands
00011E3D                 jb      short loc_11DFF ; Jump if Below (CF=1)
*/
  for (unsigned
    n = 0x0000,
    N = 0x4000;
    n < N;
  ++n)
    {
/* // Note: `edx' is `__data__N'.
00011DFF loc_11DFF:
00011DFF                 mov     eax, ebx
00011E01                 sub     eax, 2000h      ; Integer Subtraction
00011E07                 and     eax, 0FFFFh     ; Logical AND
00011E0C                 sar     eax, 4          ; Shift Arithmetic Right
00011E0F                 movsx   edx, ds:gVideo__Buffer__2[eax*2]
                                                 ; Move with Sign-Extend
*/
    unsigned
    __m =
     (n - 0x2000)
        & 0xffff;
    __m >>= 4;
    const
    int16_t
    __data__N =
    gVideo__Buffer__2[__m];
/*
00011E17                 mov     eax, edx
00011E19                 shl     eax, 2          ; Shift Logical Left
00011E1C                 add     edx, eax        ; Add
00011E1E                 shl     edx, 5          ; Shift Logical Left
00011E21                 mov     eax, edx
*/
    const
    int32_t
      data__0 =
    __data__N +
   (__data__N << 0x02);
    const
    int32_t
      data__1 =
      data__0 >> 0x05;
/*
  // Note: `edx' is  `data__1'  and  `eax' will be  `data__2'.  SBB uses the CF
  // flag,  and in this case it will be set by  SAR,  but that  is  immediately
  // overwritten by SHL (always zero).  Maybe I'm missing something.
00011E23                 sar     edx, 1Fh  ; Shift Arithmetic Right
00011E26                 shl     edx, 8    ; Shift Logical Left
00011E29                 sbb     eax, edx  ; Integer Subtraction with Borrow
*/
    const
    int32_t
      data__2 =
     (data__1 >> 0x1f)
              << 0x08;
    const
    int32_t
      data__3 =
      data__1 -
      data__2;
/*
00011E2B                 sar     eax, 8          ; Shift Arithmetic Right
00011E2E                 mov     ds:gVideo__Buffer__3[ebx*2], ax
00011E36                 inc     ebx             ; Increment by 1
*/
    gVideo__Buffer__3[n] =
      data__3 >> 0x08;
/*
00011E37 loc_11E37:
00011E37                 cmp     ebx, 4000h      ; Compare Two Operands
00011E3D                 jb      short loc_11DFF ; Jump if Below (CF=1)
*/
    }

  FDUMP__ALT(gLogic__Buffer__0, 0x1000 * sizeof(uint16_t));
  FDUMP__ALT(gLogic__Buffer__1, 0x1000 * sizeof( int16_t));
  FDUMP__ALT(gVideo__Buffer__2, 0x1000 * sizeof(uint16_t));
  FDUMP__ALT(gLogic__Buffer__2, 0x1000 * sizeof(uint16_t));
    /* // Note: It's all zero.
  FDUMP__ALT(gVideo__Buffer__3, 0x4000 * sizeof(uint16_t)) */
    ;

  FTELL(
    file__hndl);
  GGraph__LoadSpriteLookup(
    file__hndl);

  FTELL(
    file__hndl);
  GGraph__LoadSound(
    file__hndl);

  FTELL(
    file__hndl);
  GGraph__LoadFont(
    file__hndl);
  FTELL(
    file__hndl);
  GGraph__LoadPalette(
    file__hndl);

  const
  GVideo__Frame_t
    gVideo__FrameFld =
    {
   .width = 320
 , .height = 200
    };

  FTELL(
    file__hndl);
  GVideo__Pixel_t *
    gVideo__FrameFld__Canvas =
  GGraph__LoadPCX(
   &gVideo__FrameFld, file__hndl);

  GVideo__WriteBMP(
 a__gVideo__PaletteGLOBAL,
    gVideo__FrameFld__Canvas,
   &gVideo__FrameFld,
    "DUMP\\GFXBTLFD.BMP",
    gVideo__FrameFld.width,
    gVideo__FrameFld.height);
  GVideo__FrameFree(
    gVideo__FrameFld__Canvas);

  void * __attribute__((__cleanup__(GAlloc__Free)))
    gVideo__SpriteData =
  GAlloc(
    gVideo__SpriteData__Size);
  FTELL(
    file__hndl);
  fread__alt(
    gVideo__SpriteData,
    gVideo__SpriteData__Size,
    file__hndl);

  uint8_t *
  const
  __gVideo__SpriteData = (uint8_t *)
  gVideo__SpriteData;

  for (unsigned
    n = 0,
    N = gVideo__SpriteData__Size;
    n < N;
  ++n)
  __gVideo__SpriteData[n]
      ^=
    C815__MAGIC__7(n);

  FDUMP__ALT(
    gVideo__SpriteData,
    gVideo__SpriteData__Size);

#if 0
  for (
    unsigned
    n = 0x0000,
    N = 0x001d;
    n < N;
  ++n)
    {
    uint8_t *
    __gVideo__SpriteData =
      gVideo__SpriteData +
   a__gVideo__SpriteLookupGLOBAL[n].
      gVideo__SpriteOffset;
  for (
    unsigned
    m = 0x0000,
    M = 0x0040;
    m < M;
  ++m)
    {
    const
    unsigned
    __m = 0x7f - m;

    unsigned
    __0 = m * 4 + 4;
    uint32_t
  __gVideo__SpriteData__0 =
    * (uint32_t *) (
  __gVideo__SpriteData + __0);

    unsigned
    __1 = __m * 4 + 4;
    uint32_t
  __gVideo__SpriteData__1 =
    * (uint32_t *) (
  __gVideo__SpriteData + __1);

    /*
  printf("%08x %08x (%2u) <-> %08x %08x (%2u)\n",
  __gVideo__SpriteData + __0,
  __gVideo__SpriteData__0, m,
  __gVideo__SpriteData + __1,
  __gVideo__SpriteData__1, __m); */

    *(uint32_t *)
  (__gVideo__SpriteData + __1) =
      (uint32_t) ( /*
  __gVideo__SpriteData + */
  __gVideo__SpriteData__0);

    *(uint32_t *)
  (__gVideo__SpriteData + __0) =
      (uint32_t) ( /*
  __gVideo__SpriteData + */
  __gVideo__SpriteData__1);
    }
    }
#endif // ..

  size_t
    gVideo__Sprite__Width = 0;
  size_t
    gVideo__Sprite__Height = 0;

  for (unsigned
    n = 0,
    N = GVIDEO__SPRITES;
    n < N;
  ++n)
    {
    const
    GVideo__Sprite_t *
    const
      gVideo__Sprite =
    GVideo__SpriteAt(
      gVideo__SpriteData,
   a__gVideo__SpriteLookupGLOBAL, n);

    if (
      gVideo__Sprite__Width
        <
      gVideo__Sprite->width)
      gVideo__Sprite__Width =
      gVideo__Sprite->width;
    if (
      gVideo__Sprite__Height
        <
      gVideo__Sprite->height)
      gVideo__Sprite__Height =
      gVideo__Sprite->height;
    }

  assert(
    gVideo__Sprite__Width
      >= 16);
  assert(
    gVideo__Sprite__Height
      >= 16);

  const
  GVideo__Frame_t
    gVideo__Frame__SpriteMap =
  {
   .width =
    gVideo__Sprite__Width
 , .height = GVIDEO__SPRITES *
    gVideo__Sprite__Height
  };

  size_t
    gVideo__Frame__SpriteMap__CanvasSize;

  void *
    gVideo__Frame__SpriteMap__Canvas =
  GVideo__FrameAllocate(
   &gVideo__Frame__SpriteMap,
   &gVideo__Frame__SpriteMap__CanvasSize);

    /* // Note: Not enabled.
  GVideo__FrameDump__Dimensions(
   &gVideo__Frame__SpriteMap, "SpriteMap")
    */;

  void *
  __gVideo__Frame__SpriteMap__Canvas =
  gVideo__Frame__SpriteMap__Canvas;

  for (unsigned
    n = 0,
    N = GVIDEO__SPRITES;
    n < N;
  ++n)
    {
    const
    GVideo__Sprite_t *
    const
      gVideo__Sprite =
    GVideo__SpriteAt(
      gVideo__SpriteData,
   a__gVideo__SpriteLookupGLOBAL, n);

    printf("SPROBJ, %02x: width %3u, height %3u.\n", n,
      gVideo__Sprite->width,
      gVideo__Sprite->height);

    void *
    const
      gVideo__Frame__Sprite__Canvas =
    GVideo__SpriteUnpackAndAllocateFrame(
      gVideo__Sprite);
    if (
      gVideo__Frame__Sprite__Canvas)
      {
    const
    GVideo__Frame_t
      gVideo__Frame__Sprite =
    {
     .width =
      gVideo__Sprite->width
   , .height =
      gVideo__Sprite->height
    };
    const
    GVideo__Frame_t
    __gVideo__Frame__Sprite =
    {
     .width =
        AlignDWRD(
      gVideo__Frame__Sprite.
      width)
   , .height =
        AlignDWRD(
      gVideo__Frame__Sprite.
      height)
    };

      // Note: No unaligned BMP support is implemented, sorry.
    if (!
      GVideo__Frame__Equal(
       &gVideo__Frame__Sprite,
     &__gVideo__Frame__Sprite))
      {
    void *
    __gVideo__Frame__Sprite__Canvas =
    GVideo__FrameAllocate(
   &__gVideo__Frame__Sprite, NULL);
    GVideo__FrameCopy__Center(
    __gVideo__Frame__Sprite__Canvas,
   &__gVideo__Frame__Sprite,
      gVideo__Frame__Sprite__Canvas,
     &gVideo__Frame__Sprite);

    GVideo__WriteBMP(
   a__gVideo__PaletteGLOBAL,
    __gVideo__Frame__Sprite__Canvas,
   &__gVideo__Frame__Sprite,
      "DUMP\\SPROBJ%02U.BMP", n);

    GVideo__FrameFree(
    __gVideo__Frame__Sprite__Canvas);
      }
    else
    GVideo__WriteBMP(
   a__gVideo__PaletteGLOBAL,
      gVideo__Frame__Sprite__Canvas,
     &gVideo__Frame__Sprite,
      "DUMP\\SPROBJ%02U.BMP", n);

    GVideo__TextWrite(
    __gVideo__Frame__SpriteMap__Canvas,
     &gVideo__Frame__SpriteMap,
      gVideo__FontGLOBAL,
      GVIDEO__FONTTYPE__2,
      NULL,
      "IX: %02X", n);

    GVideo__FrameCopy(
    __gVideo__Frame__SpriteMap__Canvas,
     &gVideo__Frame__SpriteMap,
      gVideo__Frame__Sprite__Canvas,
     &gVideo__Frame__Sprite);

    GVideo__FrameFree(
      gVideo__Frame__Sprite__Canvas);
      }

    PMagic__AdvanceAlt(
   &__gVideo__Frame__SpriteMap__Canvas,
      gVideo__Sprite__Width *
      gVideo__Sprite__Height);
    }

  GVideo__WriteBMP(
 a__gVideo__PaletteGLOBAL,
    gVideo__Frame__SpriteMap__Canvas,
   &gVideo__Frame__SpriteMap,
    "DUMP\\SPRMAP%02U.BMP", 0);

  for (unsigned
    n = 0,
    N = GVIDEO__FONTTYPE__N;
    n < N;
  ++n)
    {
    const
    size_t
      gVideo__FontSize =
    GVideo__FontSize(n);

    static_assert(__builtin_popcount(
    GVIDEO__FONT__CHARACTERS) == 1);

    const
    size_t
      gVideo__FontMap__Row =
        1u << (__builtin_ctz(
    GVIDEO__FONT__CHARACTERS) / 2)
    , gVideo__FontMap__Col =
        1u << (__builtin_ctz(
    GVIDEO__FONT__CHARACTERS) / 2)
      ;

    const
    GVideo__Frame_t
      gVideo__Frame__FontMap =
    {
     .width =
    gVideo__FontMap__Col *
    gVideo__FontSize
   , .height =
    gVideo__FontMap__Row *
    gVideo__FontSize
    };

    GVideo__Pixel_t *
      gVideo__Frame__FontMap__Canvas =
    GVideo__FrameAllocate(
     &gVideo__Frame__FontMap, NULL);
      /* // Note: Not enabled.
    GVideo__FrameDump__Dimensions(
     &gVideo__Frame__FontMap, "FontMap") */
      ;

    GVideo__Pixel_t *
    __gVideo__Frame__FontMap__Canvas =
    gVideo__Frame__FontMap__Canvas;

  for (unsigned
    m = 0,
    M = GVIDEO__FONT__CHARACTERS;
    m < M;
  ++m)
    {
    GVideo__Frame_t
      gVideo__Frame__Font;
    void *
      gVideo__Frame__Font__Canvas =
    GVideo__FontUnpack(
      m, n,
      gVideo__FontGLOBAL,
     &gVideo__Frame__Font);

    const
    GVideo__Coord_t
      gVideo__Coord =
    {
     .x =(m %
      gVideo__FontMap__Row) *
      gVideo__FontSize
   , .y = (m /
      gVideo__FontMap__Col) *
      gVideo__FontSize
    };

    GVideo__FrameCopyAlt(
    __gVideo__Frame__FontMap__Canvas,
     &gVideo__Frame__FontMap,
      gVideo__Frame__Font__Canvas,
     &gVideo__Frame__Font,
     &gVideo__Coord);

    GVideo__FrameFree(
      gVideo__Frame__Font__Canvas);
    }

    GVideo__WriteBMP(
   a__gVideo__PaletteGLOBAL,
      gVideo__Frame__FontMap__Canvas,
     &gVideo__Frame__FontMap,
      "DUMP\\FNTMAP%02U.BMP", n);
    }

  return
    EXIT_SUCCESS;
  }
