#include "mcmp/util/bitmap.h"

#include <string.h>
#include <stdint.h>

#if defined(__GNUC__)
  #define _MCMP_NEXT_OCCUPIED_BIT_LONG(n) (__builtin_ctzll(n))
  #define _MCMP_NEXT_OCCUPIED_BIT(n)      (__builtin_ctz(n))
#endif /** ...  */

#define _MCMP_GET_OFFSET_VALUE(t, bm)  (*(t*)(bm))

#if defined(_MCMP_NEXT_OCCUPIED_BIT_LONG) \
 && defined(_MCMP_NEXT_OCCUPIED_BIT)
  bitidx_t mcmpBitmapNextAvaibleBit(
    const mcmpbitmap restrict bitmap,
    const bitidx_t            offset,
    const size_t              len
  ) {
    size_t i = (size_t)(offset / 8);
    if (i >= len)
      return -1;
    uint8_t bitrest = offset % 8;
    uint8_t inverted = ~(bitmap[i] | ((1 << bitrest) - 1));
    if (inverted)
      return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT((unsigned int)inverted));
    ++i;
    for (; i + 8 < len; i += 8) {
      uint64_t chunk = _MCMP_GET_OFFSET_VALUE(uint64_t, bitmap + i);
      uint64_t inverted = ~chunk;
      if (inverted)
        return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT_LONG(inverted));
    }
    for (; i + 4 < len; i += 4) {
      uint32_t chunk = _MCMP_GET_OFFSET_VALUE(uint32_t, bitmap + i);
      uint32_t inverted = ~chunk;
      if (inverted)
        return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT(inverted));
    }
    for (; i + 2 < len; i += 2) {
      uint16_t chunk = _MCMP_GET_OFFSET_VALUE(uint16_t, bitmap + i);
      uint16_t inverted = ~chunk;
      if (inverted)
        return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT(inverted));
    }
    for (; i < len; ++i) {
      uint8_t inverted = ~(bitmap[i]);
      if (inverted)
        return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT((unsigned int)inverted));
    }
    return -1;
  }

  bitidx_t mcmpBitmapNextOccupiedBit(
    const mcmpbitmap restrict bitmap,
    const bitidx_t            offset,
    const size_t              len
  ) {
    size_t i = (size_t)(offset / 8);
    if (i >= len)
      return -1;
    uint8_t bitrest = offset % 8;
    uint8_t chunk = bitmap[i] & ~((1 << bitrest) - 1);
    if (chunk)
      return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT((unsigned int)chunk));
    ++i;
    for (; i + 8 < len; i += 8) {
      uint64_t chunk = _MCMP_GET_OFFSET_VALUE(uint64_t, bitmap + i);
      if (chunk)
        return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT_LONG(chunk));
    }
    for (; i + 4 < len; i += 4) {
      uint32_t chunk = _MCMP_GET_OFFSET_VALUE(uint32_t, bitmap + i);
      if (chunk)
        return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT(chunk));
    }
    for (; i + 2 < len; i += 2) {
      uint16_t chunk = _MCMP_GET_OFFSET_VALUE(uint16_t, bitmap + i);
      if (chunk)
        return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT(chunk));
    }
    for (; i < len; ++i) {
      uint8_t chunk = bitmap[i];
      if (chunk)
        return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT((unsigned int)chunk));
    }
    return -1;
  }
#endif /** MCMCH_BITMAP_BUILTINS_DEF  */

void mcmpBitmapCleanOrder(
  const mcmpbitmap bitmap,
  const size_t     len
) {
  memset(bitmap, 0, len);
}

void mcmpBitmapEnableBit(
  const mcmpbitmap bitmap,
  const bitidx_t   idx
) {
  const bitidx_t byteoff = (const bitidx_t)(idx / 8);
  uint8_t bitidx  = (uint8_t)(idx % 8);
  bitmap[byteoff] |= (1 << bitidx);
}

void mcmpBitmapDisableBit(
  const mcmpbitmap bitmap,
  const bitidx_t   idx
) {
  const bitidx_t byteoff = (const bitidx_t)(idx / 8);
  uint8_t bitidx  = (uint8_t)(idx % 8);
  bitmap[byteoff] &= ~(1 << bitidx);
}

int mcmpBitmapIsEnabledBit(
  const mcmpbitmap bitmap,
  const bitidx_t   idx
) {
  const bitidx_t byteoff = (const bitidx_t)(idx / 8);
  uint8_t bitidx  = (uint8_t)(idx % 8);
  return (bitmap[byteoff] & (1 << bitidx)) != 0;
}

int mcmpBitmapIsDisabledBit(
  const mcmpbitmap bitmap,
  const bitidx_t   idx
) {
  const bitidx_t byteoff = (const bitidx_t)(idx / 8);
  uint8_t bitidx  = (uint8_t)(idx % 8);
  return (bitmap[byteoff] & (1 << bitidx)) == 0;
}