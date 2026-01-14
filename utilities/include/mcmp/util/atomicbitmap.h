#ifndef MCMP_UTIL_ATOMIC_BITMAP_H
#define MCMP_UTIL_ATOMIC_BITMAP_H
#include "mcmp/util/bitmap.h"
#include <mcmp/stat.h>
#include <stdint.h>
#include <stdio.h>

bitidx_t mcmpAtomicBitmapNextAvaibleBit  (const mcmpbitmap restrict bitmap, const bitidx_t offset, const size_t len);
bitidx_t mcmpAtomicBitmapNextOccupiedBit (const mcmpbitmap restrict bitmap, const bitidx_t offset, const size_t len);
int      mcmpAtomicBitmapIsEnabledBit    (const mcmpbitmap bitmap, const bitidx_t idx);
int      mcmpAtomicBitmapIsDisabledBit   (const mcmpbitmap bitmap, const bitidx_t idx);
mcmpst  mcmpAtomicBitmapTryEnableBit    (mcmpbitmap bitmap, const bitidx_t idx);
mcmpst  mcmpAtomicBitmapTryDisableBit   (mcmpbitmap bitmap, const bitidx_t idx);
#endif // MCMP_UTIL_ATOMIC_BITMAP_H