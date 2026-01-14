#ifndef MCMP_BITMAP_H
#define MCMP_BITMAP_H
#include <stdint.h>
#include <stdio.h>

typedef uint8_t* mcmpbitmap;
typedef int64_t bitidx_t;

bitidx_t mcmpBitmapNextAvaibleBit  (const mcmpbitmap restrict bitmap, const bitidx_t offset, const size_t len);
bitidx_t mcmpBitmapNextOccupiedBit (const mcmpbitmap restrict bitmap, const bitidx_t offset, const size_t len);
int      mcmpBitmapIsEnabledBit    (const mcmpbitmap bitmap, const bitidx_t idx);
int      mcmpBitmapIsDisabledBit   (const mcmpbitmap bitmap, const bitidx_t idx);
void     mcmpBitmapCleanOrder      (mcmpbitmap bitmap, const size_t len);
void     mcmpBitmapEnableBit       (mcmpbitmap bitmap, const bitidx_t idx);
void     mcmpBitmapDisableBit      (mcmpbitmap bitmap, const bitidx_t idx);
#endif // MCMP_BITMAP_H