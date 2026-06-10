#include "stdafx.h"
#include "MirrorImage.h"

void MirrorImg(unsigned char* imgBuf, int width, int height, int bytePerSample)
{
    std::vector<unsigned char> temp(bytePerSample, 0);
    unsigned char* tempBuf = &temp[0];
    for (int y = 0; y < height; ++y)
    {
        int base = y * width * bytePerSample;
        for (int x = 0, mid = width / 2; x < mid; ++x)
        {
            int swapAddr1 = base + x * bytePerSample;
            int swapAddr2 = base + (width - 1 - x) * bytePerSample;
            memcpy(tempBuf, &imgBuf[swapAddr1], bytePerSample);
            memcpy(&imgBuf[swapAddr1], &imgBuf[swapAddr2], bytePerSample);
            memcpy(&imgBuf[swapAddr2], tempBuf, bytePerSample);
        }
    }
}

void MirrorDepthImg(unsigned char* imgBuf, int width, int height, int bytePerSample)
{
    MirrorImg(imgBuf, width, height, bytePerSample);
}

void MirrorYUYVColorImg(unsigned char* imgBuf, int width, int height)
{
    std::vector<unsigned char> temp(4, 0);
    unsigned char* tempBuf = &temp[0];
    for (int y = 0; y < height; ++y)
    {
        int base = y * width * 2;
        for (int x = 0, mid = width / 2; x < mid; x += 2)
        {
            int swapAddr1 = base + x * 2;
            int swapAddr2 = base + (width - 2 - x) * 2;

            tempBuf[0] = imgBuf[swapAddr1 + 2];
            tempBuf[1] = imgBuf[swapAddr1 + 1];
            tempBuf[2] = imgBuf[swapAddr1];
            tempBuf[3] = imgBuf[swapAddr1 + 3];

            imgBuf[swapAddr1] = imgBuf[swapAddr2 + 2];
            imgBuf[swapAddr1 + 1] = imgBuf[swapAddr2 + 1];
            imgBuf[swapAddr1 + 2] = imgBuf[swapAddr2];
            imgBuf[swapAddr1 + 3] = imgBuf[swapAddr2 + 3];

            memcpy(&imgBuf[swapAddr2], tempBuf, 4);
        }
    }
}

void MirrorRGBColorImg(unsigned char* imgBuf, int width, int height)
{
    MirrorImg(imgBuf, width, height, 3);
}