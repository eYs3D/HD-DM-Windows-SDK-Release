#pragma once

//#define	debug_ImgRotate90_openCV_OnlyRGB	/*It is for debugging "Img Rotate 90".*/
//#define	debug_ImgResizeHalf		/*It is for debugging "Img Resize Half".*/
//#define	debug_ImgMjpegToRgb24	/*It is for debugging Img "Mjpeg To Rgb24".*/

class CImgManipulator
{
public:
    CImgManipulator() = default;
    virtual ~CImgManipulator() = default;
    int type;

	bool ImgRotateDepth180(int width, int height, int bytesPerPixel, int len,unsigned char *src, unsigned char *dst);
    bool ImgRotate90(bool clockwise, int width, int height, int bytesPerPixel, unsigned char* src, unsigned char* dst, int devicePid, unsigned short nChipID);
    //bool ImgMirror(int width, int height, int bytesPerPixel, unsigned char* src, unsigned char* dst);
    //bool ImgFlip(int width, int height, int bytesPerPixel, unsigned char* src, unsigned char* dst);
    //bool ImgCrop(int width, int height, int bytesPerPixel, unsigned char* src,
    //        int p0x, int p0y, int p1x, int p1y, unsigned char* dst);// crop image to (p1x - p0x) x (p1y - p0y) subimage
    //bool ImgResizeHalf(POINT& imgRes, int bytesPerPixel, unsigned long int& imgSize, unsigned char* src, unsigned char* dst);
	//bool ImgHalfSize(POINT& imgRes, int bytesPerPixel, unsigned long int& imgSize, unsigned char* src, unsigned char* dst);
    //bool ImgMjpegToRgb24(POINT& imgRes, unsigned char* src, unsigned char* dst);

private:
    std::vector<unsigned char> m_tempBuffer;
    std::mutex m_mutex;
};