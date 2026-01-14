#include "mcmp/util/atomicbitmap.h"

#include <mcmp/statos.h>
#include <mcmp/stat.h>
#include <string.h>

#if defined(__GNUC__)
  #define _MCMP_NEXT_OCCUPIED_BIT_LONG(n) (__builtin_ctzll(n))
  #define _MCMP_NEXT_OCCUPIED_BIT(n)      (__builtin_ctz(n))
  #define _MCMP_ATOMIC_LOAD(t, bm, i) \
    __atomic_load_n( \
      (t*)(bm + i), \
      __ATOMIC_ACQUIRE)
#endif /** __GNUC__  */

#if defined(_MCMP_NEXT_OCCUPIED_BIT_LONG) \
    && defined(_MCMP_NEXT_OCCUPIED_BIT) \
    && defined(_MCMP_ATOMIC_LOAD)
  bitidx_t mcmpAtomicBitmapNextAvaibleBit(
    const mcmpbitmap restrict bitmap,
    const bitidx_t            offset,
    const size_t              len
  ) {
    size_t i = (size_t)(offset / 8);
    if (i >= len)
      return -1;
    uint8_t bitrest = offset % 8;
    uint8_t chunk = _MCMP_ATOMIC_LOAD(uint8_t, bitmap, i);
    uint8_t inverted = ~(chunk | ((1 << bitrest) - 1));
    if (inverted)
      return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT_LONG(inverted));
    ++i;
    for (; i + 8 < len; i += 8) {
      uint64_t chunk = _MCMP_ATOMIC_LOAD(uint64_t, bitmap, i);
      uint64_t inverted = ~chunk;
      if (inverted)
        return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT_LONG(inverted));
    }
    for (; i + 4 < len; i += 4) {
      uint32_t chunk = _MCMP_ATOMIC_LOAD(uint32_t, bitmap, i);
      uint32_t inverted = ~chunk;
      if (inverted)
        return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT(inverted));
    }
    for (; i + 2 < len; i += 2) {
      uint16_t chunk = _MCMP_ATOMIC_LOAD(uint16_t, bitmap, i);
      uint16_t inverted = ~chunk;
      if (inverted)
        return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT(inverted));
    }
    for (; i < len; ++i) {
      uint8_t chunk = _MCMP_ATOMIC_LOAD(uint8_t, bitmap, i);
      uint8_t inverted = ~chunk;
      if (inverted)
        return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT((unsigned int)inverted));
    }
    return -1;
  }

  bitidx_t mcmpAtomicBitmapNextOccupiedBit(
    const mcmpbitmap restrict bitmap,
    const bitidx_t            offset,
    const size_t              len
  ) {
    size_t i = (size_t)(offset / 8);
    if (i >= len)
      return -1;
    uint8_t bitrest = offset % 8;
    uint8_t chunk = _MCMP_ATOMIC_LOAD(uint8_t, bitmap, i) & ~((1 << bitrest) - 1);
    if (chunk)
      return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT((unsigned int)chunk));
    ++i;
    for (; i + 8 < len; i += 8) {
      uint64_t chunk = _MCMP_ATOMIC_LOAD(uint64_t, bitmap, i);
      if (chunk)
        return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT_LONG(chunk));
    }
    for (; i + 4 < len; i += 4) {
      uint32_t chunk = _MCMP_ATOMIC_LOAD(uint32_t, bitmap, i);
      if (chunk)
        return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT(chunk));
    }
    for (; i + 2 < len; i += 2) {
      uint16_t chunk = _MCMP_ATOMIC_LOAD(uint16_t, bitmap, i);
      if (chunk)
        return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT(chunk));
    }
    for (; i < len; ++i) {
      uint8_t chunk = _MCMP_ATOMIC_LOAD(uint8_t, bitmap, i);
      if (chunk)
        return (bitidx_t)(i * 8 + _MCMP_NEXT_OCCUPIED_BIT((unsigned int)chunk));
    }
    return -1;
  }
#endif /** ...  */

mcmpst mcmpAtomicBitmapTryEnableBit(
  const mcmpbitmap bitmap,
  const bitidx_t   idx
) {
  const bitidx_t byteoff = (const bitidx_t)(idx / 8);
  uint8_t bitidx  = (uint8_t)(idx % 8);
  uint8_t mask = (1 << bitidx);
  uint8_t byteval = _MCMP_ATOMIC_LOAD(uint8_t, bitmap, byteoff);
  if (byteval & mask)
    MCMPST_RETURN(MCMPST_ALROP);
  uint8_t nextval = byteval | mask;
  if (__atomic_compare_exchange_n(
    (bitmap + byteoff),
    &byteval,
    nextval,
    0,
    __ATOMIC_ACQ_REL,
    __ATOMIC_RELAXED
  )) {
    MCMPST_RETURN(MCMPST_SUCCESS);
  }
  MCMPST_RETURN(MCMPST_INVOP);
}

mcmpst mcmpAtomicBitmapTryDisableBit(
  const mcmpbitmap bitmap,
  const bitidx_t   idx
) {
  const bitidx_t byteoff = (const bitidx_t)(idx / 8);
  uint8_t bitidx  = (uint8_t)(idx % 8);
  uint8_t mask = ~(1 << bitidx);
  uint8_t byteval = _MCMP_ATOMIC_LOAD(uint8_t, bitmap, byteoff);
  if ((byteval | (1 << bitidx)) == 0)
    MCMPST_RETURN(MCMPST_ALROP);
  uint8_t nextval = byteval & mask;
  if (__atomic_compare_exchange_n(
    (bitmap + byteoff),
    &byteval,
    nextval,
    0,
    __ATOMIC_ACQ_REL,
    __ATOMIC_RELAXED
  )) {
    MCMPST_RETURN(MCMPST_SUCCESS);
  }
  MCMPST_RETURN(MCMPST_INVOP);
}

int mcmpAtomicBitmapIsEnabledBit(
  const mcmpbitmap bitmap,
  const bitidx_t   idx
) {
  const bitidx_t byteoff = (const bitidx_t)(idx / 8);
  uint8_t bitidx  = (uint8_t)(idx % 8);
  uint8_t byte = __atomic_load_n(
    (uint8_t*)(bitmap + byteoff),
    __ATOMIC_ACQUIRE
  );
  return (byte & (1 << bitidx)) != 0;
}

int mcmpAtomicBitmapIsDisabledBit(
  const mcmpbitmap bitmap,
  const bitidx_t   idx
) {
  const bitidx_t byteoff = (const bitidx_t)(idx / 8);
  uint8_t bitidx  = (uint8_t)(idx % 8);
  uint8_t byte = __atomic_load_n(
    (uint8_t*)(bitmap + byteoff),
    __ATOMIC_ACQUIRE
  );
  return (byte & (1 << bitidx)) == 0;
}