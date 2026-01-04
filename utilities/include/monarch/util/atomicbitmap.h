#ifndef MONARCH_UTIL_ATOMIC_BITMAP_H
#define MONARCH_UTIL_ATOMIC_BITMAP_H
#include "monarch/util/bitmap.h"
#include <monarch/stat.h>
#include <stdint.h>
#include <stdio.h>

bitidx_t mcmchAtomicBitmapNextAvaibleBit  (const mcmchbitmap restrict bitmap, const bitidx_t offset, const size_t len);
bitidx_t mcmchAtomicBitmapNextOccupiedBit (const mcmchbitmap restrict bitmap, const bitidx_t offset, const size_t len);
int      mcmchAtomicBitmapIsEnabledBit    (const mcmchbitmap bitmap, const bitidx_t idx);
int      mcmchAtomicBitmapIsDisabledBit   (const mcmchbitmap bitmap, const bitidx_t idx);
mcmchst  mcmchAtomicBitmapTryEnableBit    (mcmchbitmap bitmap, const bitidx_t idx);
mcmchst  mcmchAtomicBitmapTryDisableBit   (mcmchbitmap bitmap, const bitidx_t idx);
#endif // MONARCH_UTIL_ATOMIC_BITMAP_H