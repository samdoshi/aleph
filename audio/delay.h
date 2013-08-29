/*
delay.h
aleph-audio

delay line object interface

 */
#ifndef _ALEPH_LINES_DELAYLINE_H_
#define _ALEPH_LINES_DELAYLINE_H_

#include "buffer.h"
#include "types.h"

// delay line data structure
typedef struct _delayLine {
  //-- audio buffer class
  audioBuffer buffer;
  //-- read and write taps (non-interpolated)
  bufferTapN tapRd;
  bufferTapN tapWr;
  // flag to synchronize read tap with write tap
  u8 sync;
  // flag to toggle writing of new data
  u8 write;
  // level of existing data to mix with new data
  // negative == 1
  fract32 preLevel;
  // flags to run or pause each tap (phasor)
  u8 runRd;
  u8 runWr;
} delayLine;

// initialize with pointer to audio buffer data
extern void delay_init(delayLine* dl, fract32* bufData, u32 frames);
// get next value given input
extern fract32 delay_next(delayLine* dl, fract32 in);
// set loop endpoint in seconds
extern void delay_set_loop(delayLine* dl, fix16 sec);
// set delay time in seconds
extern void delay_set_delay(delayLine* dl, fix16 sec); 
// set synchronization flag
// extern void delay_set_sync(delayLine* dl, u8 sync);
// set erase level
extern void delay_set_pre(delayLine* dl, fract32 pre);
// set write level
extern void delay_set_write(delayLine* dl, u8 write);
// set read head rate
extern void delay_set_rate(delayLine* dl, fix16 rate);

#endif // h guard
