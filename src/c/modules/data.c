#include "data.h"

uint key = 3;

char s_title_arr[8] = "00:00:0";

uint64_t getTime()
{
  return time(NULL) * 1000 + time_ms(NULL, NULL);
}

void emptyArr(char o[], uint max)
{
  for(uint i = 0; i<max; i++) o[i] = '\0';
}

void timer_display(uint64_t ms, char o[], uint i){
  uint s = ms / 1000 % 60;
  uint m = ms / 1000 / 60 % 60;
  o[i + 7] = '\0';
  o[i + 6] = ms / 100 % 10 + '0';
  o[i + 5] = ':';
  o[i + 4] = s % 10 + '0';
  o[i + 3] = s / 10  % 6 + '0';
  o[i + 2] = ':';
  o[i + 1] = m % 10 + '0';
  o[i + 0] = m / 10 % 6 + '0';
}

void multime()
{
  if(gD.num < 1) return;
  timer_display(getTime() - gD.laps[gD.num-1], s_multime_arr, 0);
  s_multime_arr[7] = ' ';
  timer_display(getTime() - gD.start, s_multime_arr, 8);
  s_multime_arr[15] = '\n';
  for(uint i=1; i<gD.num; i++){
    timer_display(gD.laps[gD.num -i] - gD.laps[gD.num -i-1], s_multime_arr, i*16);
    s_multime_arr[7 + i*16] = ' ';
    timer_display(gD.laps[gD.num -i] - gD.start, s_multime_arr, 8+i*16);
    s_multime_arr[15 + i*16] = '\n';
  }
  timer_display(gD.laps[0] - gD.start, s_multime_arr, gD.num* 16);
  s_multime_arr[7 + gD.num*16] = ' ';
  timer_display(gD.laps[0] - gD.start, s_multime_arr, 8+gD.num* 16);
}