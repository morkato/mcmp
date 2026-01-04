#include <string.h>

#include <monarch/statos.h>
#include "monarch/util/atomicbitmap.h"
#include "monarch/stat.h"

#if defined(__GNUC__)
  #define MCMCHBITMAP_NEXT_OCCUPIED_BIT_LONG(n) (__builtin_ctzll(n))
  #define MCMCHBITMAP_NEXT_OCCUPIED_BIT(n)      (__builtin_ctz(n))
  #define MCMCHBITMAP_ATOMIC_LOAD(t, bm, i) \
    __atomic_load_n( \
      (t*)(bm + i), \
      __ATOMIC_ACQUIRE)
  #define MCMCHBITMAP_BUILTINS_DEF              1
#else
  #define MCMCHBITMAP_GET_OFFSET_VALUE(t, bm)  (*(t*)(bm))
#endif /** ...  */

#ifdef MCMCHBITMAP_BUILTINS_DEF
  bitidx_t mcmchAtomicBitmapNextAvaibleBit(const mcmchbitmap restrict bitmap, const bitidx_t offset, const size_t len) {
    size_t i = (size_t)(offset / 8);
    if (i >= len)
      return -1;
    uint8_t bitrest = offset % 8;
    uint8_t chunk = MCMCHBITMAP_ATOMIC_LOAD(uint8_t, bitmap, i);
    uint8_t inverted = ~(chunk | ((1 << bitrest) - 1));
    if (inverted)
      return (bitidx_t)(i * 8 + MCMCHBITMAP_NEXT_OCCUPIED_BIT_LONG(inverted));
    ++i;
    for (; i + 8 < len; i += 8) {
      uint64_t chunk = MCMCHBITMAP_ATOMIC_LOAD(uint64_t, bitmap, i);
      uint64_t inverted = ~chunk;
      if (inverted)
        return (bitidx_t)(i * 8 + MCMCHBITMAP_NEXT_OCCUPIED_BIT_LONG(inverted));
    }
    for (; i + 4 < len; i += 4) {
      uint32_t chunk = MCMCHBITMAP_ATOMIC_LOAD(uint32_t, bitmap, i);
      uint32_t inverted = ~chunk;
      if (inverted)
        return (bitidx_t)(i * 8 + MCMCHBITMAP_NEXT_OCCUPIED_BIT(inverted));
    }
    for (; i + 2 < len; i += 2) {
      uint16_t chunk = MCMCHBITMAP_ATOMIC_LOAD(uint16_t, bitmap, i);
      uint16_t inverted = ~chunk;
      if (inverted)
        return (bitidx_t)(i * 8 + MCMCHBITMAP_NEXT_OCCUPIED_BIT(inverted));
    }
    for (; i < len; ++i) {
      uint8_t chunk = MCMCHBITMAP_ATOMIC_LOAD(uint8_t, bitmap, i);
      uint8_t inverted = ~chunk;
      if (inverted)
        return (bitidx_t)(i * 8 + MCMCHBITMAP_NEXT_OCCUPIED_BIT((unsigned int)inverted));
    }
    return -1;
  }

  bitidx_t mcmchAtomicBitmapNextOccupiedBit(const mcmchbitmap restrict bitmap, const bitidx_t offset, const size_t len) {
    size_t i = (size_t)(offset / 8);
    if (i >= len)
      return -1;
    uint8_t bitrest = offset % 8;
    uint8_t chunk = MCMCHBITMAP_ATOMIC_LOAD(uint8_t, bitmap, i) & ~((1 << bitrest) - 1);
    if (chunk)
      return (bitidx_t)(i * 8 + MCMCHBITMAP_NEXT_OCCUPIED_BIT((unsigned int)chunk));
    ++i;
    for (; i + 8 < len; i += 8) {
      uint64_t chunk = MCMCHBITMAP_ATOMIC_LOAD(uint64_t, bitmap, i);
      if (chunk)
        return (bitidx_t)(i * 8 + MCMCHBITMAP_NEXT_OCCUPIED_BIT_LONG(chunk));
    }
    for (; i + 4 < len; i += 4) {
      uint32_t chunk = MCMCHBITMAP_ATOMIC_LOAD(uint32_t, bitmap, i);
      if (chunk)
        return (bitidx_t)(i * 8 + MCMCHBITMAP_NEXT_OCCUPIED_BIT(chunk));
    }
    for (; i + 2 < len; i += 2) {
      uint16_t chunk = MCMCHBITMAP_ATOMIC_LOAD(uint16_t, bitmap, i);
      if (chunk)
        return (bitidx_t)(i * 8 + MCMCHBITMAP_NEXT_OCCUPIED_BIT(chunk));
    }
    for (; i < len; ++i) {
      uint8_t chunk = MCMCHBITMAP_ATOMIC_LOAD(uint8_t, bitmap, i);
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

mcmchst mcmchAtomicBitmapTryEnableBit(const mcmchbitmap bitmap, const bitidx_t idx) {
  const bitidx_t byteoff = (const bitidx_t)(idx / 8);
  uint8_t bitidx  = (uint8_t)(idx % 8);
  uint8_t mask = (1 << bitidx);
  uint8_t byteval = MCMCHBITMAP_ATOMIC_LOAD(uint8_t, bitmap, byteoff);
  if (byteval & mask)
    MCMCHST_RETURN(MCMCHST_ALROP);
  uint8_t nextval = byteval | mask;
  if (__atomic_compare_exchange_n(
    (bitmap + byteoff),
    &byteval,
    nextval,
    0,
    __ATOMIC_ACQ_REL,
    __ATOMIC_RELAXED
  )) {
    MCMCHST_RETURN(MCMCHST_SUCCESS);
  }
  MCMCHST_RETURN(MCMCHST_INVOP);
}

mcmchst mcmchAtomicBitmapTryDisableBit(const mcmchbitmap bitmap, const bitidx_t idx) {
  const bitidx_t byteoff = (const bitidx_t)(idx / 8);
  uint8_t bitidx  = (uint8_t)(idx % 8);
  uint8_t mask = ~(1 << bitidx);
  uint8_t byteval = MCMCHBITMAP_ATOMIC_LOAD(uint8_t, bitmap, byteoff);
  if ((byteval | (1 << bitidx)) == 0)
    MCMCHST_RETURN(MCMCHST_ALROP);
  uint8_t nextval = byteval & mask;
  if (__atomic_compare_exchange_n(
    (bitmap + byteoff),
    &byteval,
    nextval,
    0,
    __ATOMIC_ACQ_REL,
    __ATOMIC_RELAXED
  )) {
    MCMCHST_RETURN(MCMCHST_SUCCESS);
  }
  MCMCHST_RETURN(MCMCHST_INVOP);
}

int mcmchAtomicBitmapIsEnabledBit(const mcmchbitmap bitmap, const bitidx_t idx) {
  const bitidx_t byteoff = (const bitidx_t)(idx / 8);
  uint8_t bitidx  = (uint8_t)(idx % 8);
  uint8_t byte = __atomic_load_n(
    (uint8_t*)(bitmap + byteoff),
    __ATOMIC_ACQUIRE
  );
  return (byte & (1 << bitidx)) != 0;
}

int mcmchAtomicBitmapIsDisabledBit(const mcmchbitmap bitmap, const bitidx_t idx) {
  const bitidx_t byteoff = (const bitidx_t)(idx / 8);
  uint8_t bitidx  = (uint8_t)(idx % 8);
  uint8_t byte = __atomic_load_n(
    (uint8_t*)(bitmap + byteoff),
    __ATOMIC_ACQUIRE
  );
  return (byte & (1 << bitidx)) == 0;
}