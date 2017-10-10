#pragma once         // Prevent errors by being included multiple times

#include <pebble.h>  // Pebble SDK symbols

char s_title_arr[8], s_multime_arr[16 * 8];

typedef struct {
  uint64_t start, pause, laps[8];
  uint num;
} Data;

Data gD;

uint key;

uint64_t getTime();

void multime();

void emptyArr(char o[], uint max);

void timer_display(uint64_t ms, char o[], uint i);