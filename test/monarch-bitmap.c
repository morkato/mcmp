#ifndef _POSIX_C_SOURCE
  #define _POSIX_C_SOURCE 200809L
#endif

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

#include <monarch/bitmap.h>

#define PERFORM_BITMAP 128
#define BITMAP_SIZE    64

void perform_bitmap_next_ocuppied(mcmchbitmap bitmap, int performer) {
  printf("Percorrendo o bitmap para bits ocupados %i vezes concecutivas.\n", performer);
  struct timespec start;
  struct timespec end;
  long long mintime = LLONG_MAX;
  long long maxtime = 0LL;
  long long accumulated = 0LL;
  for (int i = 0; i < performer; ++i) {
    long long snapshot;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (bitidx_t bitidx = mcmchBitmapNextOccupiedBit(bitmap, 0, BITMAP_SIZE); bitidx != -1; bitidx = mcmchBitmapNextOccupiedBit(bitmap, bitidx + 1, BITMAP_SIZE)) (void)0;
    clock_gettime(CLOCK_MONOTONIC, &end);
    snapshot = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
    accumulated += snapshot;
    if (snapshot < mintime)
      mintime = snapshot;
    if (snapshot > maxtime)
      maxtime = snapshot;
  }
  double mintime_us = (double)mintime / 1000.0;
  double maxtime_us = (double)maxtime / 1000.0;
  double average_us = (double)accumulated / performer / 1000.0;
  printf("Resultados da analise de desempenho:\n");
  printf("  - Tempo minimo: %.2f us\n", mintime_us);
  printf("  - Tempo maximo: %.2f us\n", maxtime_us);
  printf("  - Tempo medio:  %.2f us\n", average_us);
}

void perform_bitmap_next_avaible(mcmchbitmap bitmap, int performer) {
  printf("Percorrendo o bitmap para bits livers %i vezes concecutivas.\n", performer);
  struct timespec start;
  struct timespec end;
  long long mintime = LLONG_MAX;
  long long maxtime = 0LL;
  long long accumulated = 0LL;
  for (int i = 0; i < performer; ++i) {
    long long snapshot;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (bitidx_t bitidx = mcmchBitmapNextAvaibleBit(bitmap, 0, BITMAP_SIZE); bitidx != -1; bitidx = mcmchBitmapNextAvaibleBit(bitmap, bitidx + 1, BITMAP_SIZE)) (void)0;
    clock_gettime(CLOCK_MONOTONIC, &end);
    snapshot = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
    accumulated += snapshot;
    if (snapshot < mintime)
      mintime = snapshot;
    if (snapshot > maxtime)
      maxtime = snapshot;
  }
  double mintime_us = (double)mintime / 1000.0;
  double maxtime_us = (double)maxtime / 1000.0;
  double average_us = (double)accumulated / performer / 1000.0;
  printf("Resultados da analise de desempenho:\n");
  printf("  - Tempo minimo: %.2f us\n", mintime_us);
  printf("  - Tempo maximo: %.2f us\n", maxtime_us);
  printf("  - Tempo medio:  %.2f us\n", average_us);
}

int main() {
  uint8_t bitmap[BITMAP_SIZE];
  mcmchBitmapCleanOrder(bitmap, BITMAP_SIZE);
  mcmchBitmapEnableBit(bitmap, 1); printf("Ativando o BIT 1.\n");
  mcmchBitmapEnableBit(bitmap, 0); printf("Ativando o BIT 0.\n");
  mcmchBitmapEnableBit(bitmap, 10); printf("Ativando o BIT 10.\n");
  
  printf("Verificando se os bits foram realmentes acesos.\n");
  
  printf("BIT 1: "); assert(mcmchBitmapIsEnabledBit(bitmap, 1));   assert(!mcmchBitmapIsDisabledBit(bitmap, 1));  printf("OK\n");
  printf("BIT 0: "); assert(mcmchBitmapIsEnabledBit(bitmap, 0));   assert(!mcmchBitmapIsDisabledBit(bitmap, 0));  printf("OK\n");
  printf("BIT 10: "); assert(mcmchBitmapIsEnabledBit(bitmap, 10)); assert(!mcmchBitmapIsDisabledBit(bitmap, 10)); printf("OK\n");
  
  printf("Verificando a eficiencia do bitmap. Começando chamadas recursivas para ver bit ocupados.\n");
  perform_bitmap_next_ocuppied(bitmap, PERFORM_BITMAP);
  printf("Verificando a eficiencia do bitmap. Começando chamadas recursivas para ver bit livers.\n");
  perform_bitmap_next_avaible(bitmap, PERFORM_BITMAP);
  return 0;
}