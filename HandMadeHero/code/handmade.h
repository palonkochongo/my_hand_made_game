#pragma once

struct gameOffScreenBuffer{
void *memory;
int width;
int height;
int pitch;
};

static void gameUpdateAndRender(gameOffScreenBuffer *buffer, int blueOffset, int greenOffset);
