#include <string.h>

#include "monarch/util/bitmap.h"

#if defined(__GNUC__)
  #define MCMCHBITMAP_NEXT_OCCUPIED_BIT_LONG(n) (__builtin_ctzll(n))
  #define MCMCHBITMAP_NEXT_OCCUPIED_BIT(n)      (__builtin_ctz(n))
  #define MCMCHBITMAP_GET_OFFSET_VALUE(t, bm)  (*(t*)(bm))
  #define MCMCHBITMAP_BUILTINS_DEF              1
#else
  #define MCMCHBITMAP_GET_OFFSET_VALUE(t, bm)  (*(t*)(bm))
#endif /** ...  */

#ifdef MCMCHBITMAP_BUILTINS_DEF
  bitidx_t mcmchBitmapNextAvaibleBit(const mcmchbitmap restrict bitmap, const bitidx_t offset, const size_t len) {
    size_t i = (size_t)(offset / 8);
    if (i >= len)
      return -1;
    uint8_t bitrest = offset % 8;
    uint8_t inverted = ~(bitmap[i] | ((1 << bitrest) - 1));
    if (inverted)
      return (bitidx_t)(i * 8 + MCMCHBITMAP_NEXT_OCCUPIED_BIT((unsigned int)inverted));
    ++i;
    for (; i + 8 < len; i += 8) {
      uint64_t chunk = MCMCHBITMAP_GET_OFFSET_VALUE(uint64_t, bitmap + i);
      uint64_t inverted = ~chunk;
      if (inverted)
        return (bitidx_t)(i * 8 + MCMCHBITMAP_NEXT_OCCUPIED_BIT_LONG(inverted));
    }
    for (; i + 4 < len; i += 4) {
      uint32_t chunk = MCMCHBITMAP_GET_OFFSET_VALUE(uint32_t, bitmap + i);
      uint32_t inverted = ~chunk;
      if (inverted)
        return (bitidx_t)(i * 8 + MCMCHBITMAP_NEXT_OCCUPIED_BIT(inverted));
    }
    for (; i + 2 < len; i += 2) {
      uint16_t chunk = MCMCHBITMAP_GET_OFFSET_VALUE(uint16_t, bitmap + i);
      uint16_t inverted = ~chunk;
      if (inverted)
        return (bitidx_t)(i * 8 + MCMCHBITMAP_NEXT_OCCUPIED_BIT(inverted));
    }
    for (; i < len; ++i) {
      uint8_t inverted = ~(bitmap[i]);
      if (inverted)
        return (bitidx_t)(i * 8 + MCMCHBITMAP_NEXT_OCCUPIED_BIT((unsigned int)inverted));
    }
    return -1;
  }

  bitidx_t mcmchBitmapNextOccupiedBit(const mcmchbitmap restrict bitmap, const bitidx_t offset, const size_t len) {
    size_t i = (size_t)(offset / 8);
    if (i >= len)
      return -1;
    uint8_t bitrest = offset % 8;
    uint8_t chunk = bitmap[i] & ~((1 << bitrest) - 1);
    if (chunk)
      return (bitidx_t)(i * 8 + MCMCHBITMAP_NEXT_OCCUPIED_BIT((unsigned int)chunk));
    ++i;
    for (; i + 8 < len; i += 8) {
      uint64_t chunk = MCMCHBITMAP_GET_OFFSET_VALUE(uint64_t, bitmap + i);
      if (chunk)
        return (bitidx_t)(i * 8 + MCMCHBITMAP_NEXT_OCCUPIED_BIT_LONG(chunk));
    }
    for (; i + 4 < len; i += 4) {
      uint32_t chunk = MCMCHBITMAP_GET_OFFSET_VALUE(uint32_t, bitmap + i);
      if (chunk)
        return (bitidx_t)(i * 8 + MCMCHBITMAP_NEXT_OCCUPIED_BIT(chunk));
    }
    for (; i + 2 < len; i += 2) {
      uint16_t chunk = MCMCHBITMAP_GET_OFFSET_VALUE(uint16_t, bitmap + i);
      if (chunk)
        return (bitidx_t)(i * 8 + MCMCHBITMAP_NEXT_OCCUPIED_BIT(chunk));
    }
    for (; i < len; ++i) {
      uint8_t chunk = bitmap[i];
      if (chunk)
        return (bitidx_t)(i * 8 + MCMCHBITMAP_NEXT_OCCUPIED_BIT((unsigned int)chunk));
    }
    return -1;
  }

  #undef MCMCHBITMAP_BUILTINS_DEF
  #undef MCMCHBITMAP_NEXT_OCCUPIED_BIT_LONG
  #undef MCMCHBITMAP_NEXT_OCCUPIED_BIT
  #undef MCMCHBITMAP_GET_OFFSET_VALUE
#endif /** MCMCH_BITMAP_BUILTINS_DEF  */

void mcmchBitmapCleanOrder(const mcmchbitmap bitmap, const size_t len) {
  memset(bitmap, 0, len);
}

void mcmchBitmapEnableBit(const mcmchbitmap bitmap, const bitidx_t idx) {
  const bitidx_t byteoff = (const bitidx_t)(idx / 8);
  uint8_t bitidx  = (uint8_t)(idx % 8);
  bitmap[byteoff] |= (1 << bitidx);
}

void mcmchBitmapDisableBit(const mcmchbitmap bitmap, const bitidx_t idx) {
  const bitidx_t byteoff = (const bitidx_t)(idx / 8);
  uint8_t bitidx  = (uint8_t)(idx % 8);
  bitmap[byteoff] &= ~(1 << bitidx);
}

int mcmchBitmapIsEnabledBit(const mcmchbitmap bitmap, const bitidx_t idx) {
  const bitidx_t byteoff = (const bitidx_t)(idx / 8);
  uint8_t bitidx  = (uint8_t)(idx % 8);
  return (bitmap[byteoff] & (1 << bitidx)) != 0;
}

int mcmchBitmapIsDisabledBit(const mcmchbitmap bitmap, const bitidx_t idx) {
  const bitidx_t byteoff = (const bitidx_t)(idx / 8);
  uint8_t bitidx  = (uint8_t)(idx % 8);
  return (bitmap[byteoff] & (1 << bitidx)) == 0;
}