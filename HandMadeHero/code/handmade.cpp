#include "handmade.h"
#include <stdint.h>

static void renderWierdGradient(gameOffScreenBuffer *buffer, int blueOffset, int greenOffset){

  int width = buffer->width;
  int height = buffer->height;
  uint8_t *row = (uint8_t *)buffer->memory;
  for(int y = 0 ; y<buffer->height ; y++){
    uint8_t *pixel = (uint8_t *)row;
    for(int x=0 ; x<buffer->width ; x++){
      *pixel = (int64_t)y + blueOffset; //blue
      ++pixel;
      *pixel = (int64_t)x + greenOffset;//green
      ++pixel;
        *pixel = (int64_t)0;//red
      ++pixel;
      *pixel = 0;
      ++pixel;
    }
    row += buffer->pitch;
  }
}

static void gameUpdateAndRender(gameOffScreenBuffer *buffer, int blueOffset, int greenOffset){
    renderWierdGradient(buffer, blueOffset, greenOffset);
}
