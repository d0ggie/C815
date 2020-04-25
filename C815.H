#ifndef __C815_H
#define __C815_H

// Note:  All  these magic numbers are  hard coded  in the code.  There  are no
// other magic number except these and these are all used.
#define C815__MAGIC__1 0xf5d43dd7u
// Note: I have no idea how these magic numbers where chosen. ``EA''  could be
// interpreted as ``Ennen Ajanlasku[n alkua]'', i.e.  AD  in  Finnish language.
// Nero,  Roman emperor,  was born 37 AD and he became a Roman Consul in 55 AD.
// However,  bitwise negate of `0x37ea' is `0xc815'.  Better still,  this could
// read ``[se] [kas<i>] [i] [s]'',  ``sekaisin''  which means something between
// confused and perplexed.  That would be a fitting key for an XOR obfuscation.
#define C815__MAGIC__2 0x000037eau
#define C815__MAGIC__3 0x33693cd0u
#define C815__MAGIC__4 0x0041fbe3u

static
inline
uint32_t
C815__MAGIC__5(
  unsigned
    n
  )
  {
  return
   (n * 0x03)
 + (n + 0x84)
    ;
  }

static
inline
uint8_t
C815__MAGIC__6(
  unsigned
    n
  )
  {
  return
   (n * 0x03)
  +     0x9d
    ;
  }

static
inline
uint8_t
C815__MAGIC__7(
  unsigned
    n
  )
  {
  return
   (n * 0x05)
 + (n + 0x9a)
   ;
  }

#endif//__C815_H