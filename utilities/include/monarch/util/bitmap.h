#ifndef MONARCH_BITMAP_H
#define MONARCH_BITMAP_H
#include <stdint.h>
#include <stdio.h>

typedef uint8_t* mcmchbitmap;
typedef int64_t bitidx_t;

bitidx_t mcmchBitmapNextAvaibleBit  (const mcmchbitmap restrict bitmap, const bitidx_t offset, const size_t len);
bitidx_t mcmchBitmapNextOccupiedBit (const mcmchbitmap restrict bitmap, const bitidx_t offset, const size_t len);
int      mcmchBitmapIsEnabledBit    (const mcmchbitmap bitmap, const bitidx_t idx);
int      mcmchBitmapIsDisabledBit   (const mcmchbitmap bitmap, const bitidx_t idx);
void     mcmchBitmapCleanOrder      (mcmchbitmap bitmap, const size_t len);
void     mcmchBitmapEnableBit       (mcmchbitmap bitmap, const bitidx_t idx);
void     mcmchBitmapDisableBit      (mcmchbitmap bitmap, const bitidx_t idx);
#endif // MONARCH_BITMAP_H