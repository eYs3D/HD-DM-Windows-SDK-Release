#pragma once

void MirrorDepthImg(unsigned char* imgBuf, int width, int height, int bytePerSample);
void MirrorYUYVColorImg(unsigned char* imgBuf, int width, int height);
void MirrorRGBColorImg(unsigned char* imgBuf, int width, int height);