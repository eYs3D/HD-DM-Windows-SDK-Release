#include "stdafx.h"
#include "ImgManipulator.h"
#include "eSPDI_common.h"
#include "eys_imgproc/img_alter.h"
#include "eys_imgproc/img_format.h"

#pragma comment(lib, "eys_world.lib")

bool CImgManipulator::ImgRotateDepth180(
	int width,
	int height,
	int bytesPerPixel, 
	int len,
	unsigned char *src, 
	unsigned char *dst )
{
	int imgSize = 0;
	int dstSize = 0;
	if (bytesPerPixel < 0) {
		printf("%s,bytesPerPixel incorrect\n", __func__);
		return false;
	}
	imgSize = width * height * bytesPerPixel;
	if (imgSize > len) {
		printf("%s, Image size incorrect = %d, but len = %d\n", __func__, imgSize, len);
		return false;
	}
	if (bytesPerPixel == 2 || bytesPerPixel == 1) {
		unsigned char *srcTmp = (unsigned char*)malloc(len);
		memcpy(srcTmp, src, len);
		for (int i = imgSize; i >= bytesPerPixel; i -= bytesPerPixel) {
			memcpy(&dst[dstSize], &(srcTmp[i - bytesPerPixel]), bytesPerPixel);
			dstSize += bytesPerPixel;
		}
		free(srcTmp);
	}
	else {
		return false;
	}
	return true;
}

bool CImgManipulator::ImgRotate90(
	bool clockwise, 
	int width, 
	int height, 
	int bytesPerPixel, 
	unsigned char* src, 
	unsigned char* dst, 
	int devicePid,
	unsigned short nChipID)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    const int squareEdge = (width > height) ? width : height;
    const size_t bufferSize = (size_t)(squareEdge * squareEdge * bytesPerPixel);
    if (m_tempBuffer.size() < bufferSize)
    {
        m_tempBuffer.resize(bufferSize);
    }

#ifdef debug_ImgRotate90_openCV_OnlyRGB
	TRACE("src_width=%d,src_height=%d\n", width, height); 
	int key;

	if (bytesPerPixel == 3) {			//COLOR_RGB 
		eys::img_io::Img_Show_CV(width, height, src, "Color_SRC_Img", 0.5f, key);
	}
	else if (bytesPerPixel == 2) {		//Depth_YUV to Depth_RGB
		unsigned char *img_dst = new unsigned char[width*height * 3];
		eys::img_format::YUV_422_To_RGB_CV(width, height, src, img_dst);
		eys::img_io::Img_Show_CV(width, height, img_dst, "Depth_SRC_Img", 0.5f, key);
		free(img_dst);
	}
#endif

	if (ETronDI_PID_8054 == devicePid || ETronDI_PID_8040S == devicePid) {
		unsigned char *ptr = src;
		for (int i = 0; i < height; i++) {
			memcpy(&m_tempBuffer[0] + i*squareEdge*bytesPerPixel, ptr, width*bytesPerPixel);
			ptr += width*bytesPerPixel;
		}
	}
	else if(ETronDI_PID_8040W == devicePid) {
		memcpy(&m_tempBuffer[0], src, width * height * bytesPerPixel);
	}

#ifdef debug_ImgRotate90_openCV_OnlyRGB 
	if (bytesPerPixel == 3) {			//COLOR_RGB 
		eys::img_io::Img_Show_CV(squareEdge, squareEdge, &m_tempBuffer[0], "Color_squareEdge_Img", 0.5f, key);
	}
	else if (bytesPerPixel == 2) {		//Depth_YUV to Depth_RGB
		unsigned char *img_dst = new unsigned char[squareEdge*squareEdge*3];
		eys::img_format::YUV_422_To_RGB_CV(squareEdge, squareEdge, &m_tempBuffer[0], img_dst);
		eys::img_io::Img_Show_CV(squareEdge, squareEdge, img_dst, "Depth_squareEdge_Img", 0.5f, key);
		free(img_dst);
	}
#endif	

    //int ret = eys::img_alter::Img_Rot_CV(squareEdge, squareEdge, (const float)0.0, (const float)0.0, clockwise ? (const float)90.0 : (const float)-90.0, bytesPerPixel,
    //    &m_tempBuffer[0], &m_tempBuffer[0]);
    int ret = eys::img_alter::Img_Rot90_CV(squareEdge, squareEdge, bytesPerPixel, &m_tempBuffer[0], clockwise ? 1 : 0, &m_tempBuffer[0]);
    if (ret != 1)
    {
        OutputDebugString((std::wstring(L"eys::img_alter::Img_Rot_CV failed. (error code: ") + std::to_wstring(ret) + L")").c_str());
        return false;
    }

#ifdef debug_ImgRotate90_openCV_OnlyRGB 
	if (bytesPerPixel == 3) {			//COLOR_RGB 
		eys::img_io::Img_Show_CV(squareEdge, squareEdge, &m_tempBuffer[0], "Color_Img_Rot_CV_Img", 0.5f, key);
	}
	else if (bytesPerPixel == 2) {		//Depth_YUV to Depth_RGB
		unsigned char *img_dst = new unsigned char[squareEdge*squareEdge * 3];
		eys::img_format::YUV_422_To_RGB_CV(squareEdge, squareEdge, &m_tempBuffer[0], img_dst);
		eys::img_io::Img_Show_CV(squareEdge, squareEdge, img_dst, "Depth_Img_Rot_CV_Img", 0.5f, key);
		free(img_dst);
	}
#endif

	if (ETronDI_PID_8054 == devicePid || ETronDI_PID_8040S == devicePid) {
		if (nChipID == 0x15)
		{
			ret = eys::img_alter::Img_Crop_CV(squareEdge, squareEdge,
				clockwise ? squareEdge - height : 0,
				clockwise ? 0 : squareEdge - width,
				clockwise ? height - 1 : squareEdge - 1,
				clockwise ? width - 1 : squareEdge - 1,
				bytesPerPixel, &m_tempBuffer[0], &m_tempBuffer[0]);
		}
		else if(nChipID == 0x29)
		{
			ret = eys::img_alter::Img_Crop_CV(squareEdge, squareEdge,
				clockwise ? squareEdge - height : 0, 0,
				clockwise ? squareEdge - 1 : height - 1, clockwise ? squareEdge - 1 : width - 1,
				bytesPerPixel, &m_tempBuffer[0], &m_tempBuffer[0]);
		}
	}
	else if(ETronDI_PID_8040W == devicePid) {
		ret = eys::img_alter::Img_Crop_CV(squareEdge, squareEdge,
			clockwise ? squareEdge - height : 0, 0,
			clockwise ? squareEdge - 1 : height - 1, clockwise ? squareEdge - 1 : width - 1,
			bytesPerPixel, &m_tempBuffer[0], dst);
	}
    if (ret != 1)
    {
        OutputDebugString((std::wstring(L"eys::img_alter::Img_Crop_CV failed. (error code: ") + std::to_wstring(ret) + L")").c_str());
        return false;
    }

#ifdef debug_ImgRotate90_openCV_OnlyRGB 
	if (bytesPerPixel == 3) {			//COLOR_RGB 
		eys::img_io::Img_Show_CV(height, width, &m_tempBuffer[0], "Color_Img_Crop_CV", 0.5f, key);
	}
	else if (bytesPerPixel == 2) {		//Depth_YUV to Depth_RGB
		unsigned char *img_dst = new unsigned char[squareEdge*squareEdge * 3];
		eys::img_format::YUV_422_To_RGB_CV(height, width, &m_tempBuffer[0], img_dst);
		eys::img_io::Img_Show_CV(height, width, img_dst, "Depth_Img_Crop_CV", 0.5f, key);
		free(img_dst);
	}
	TRACE("dst_width=%d,dst_height=%d\n", height, width); 
#endif

	memcpy(dst, &m_tempBuffer[0], width * height * bytesPerPixel);

    return true;
}

//bool CImgManipulator::ImgMirror(int width, int height, int bytesPerPixel, unsigned char* src, unsigned char* dst)
//{
//    std::lock_guard<std::mutex> lock(m_mutex);
//
//    if (bytesPerPixel == 1 && type == 1) // for 1 byte depth buffer 
//        width *= 2;
//	
//    int ret = eys::img_alter::Img_Flip_CV(width, height, bytesPerPixel, src, 1, dst);
//    if (ret != 1)
//    {
//        OutputDebugString((std::wstring(L"eys::img_alter::Img_Flip_CV failed. (error code: ") + std::to_wstring(ret) + L")").c_str());
//        return false;
//    }
//
//    if (bytesPerPixel == 2 && type == 0) // this is for YUYV color, workaround for OpenCV can not fully support
//    {
//    	unsigned char data;
//        for (int i = 0; i < width*height*bytesPerPixel; i++)
//        {
//            if (((i+1) % 4) ==0)
//            {
//            data = dst[i];
//            dst[i] = dst[i-2];
//            dst[i-2] = data;
//            }
//        }
//    }
//    return true;
//}

//bool CImgManipulator::ImgFlip(int width, int height, int bytesPerPixel, unsigned char* src, unsigned char* dst)
//{
//    std::lock_guard<std::mutex> lock(m_mutex);
//
//    int ret = eys::img_alter::Img_Flip_CV(width, height, bytesPerPixel, src, 0, dst);
//    if (ret != 1)
//    {
//        OutputDebugString((std::wstring(L"eys::img_alter::Img_Flip_CV failed. (error code: ") + std::to_wstring(ret) + L")").c_str());
//        return false;
//    }
//
//#ifdef debug_ImgFlip
//	int key;
//	eys::img_io::Img_Show_CV(width, height, dst, "ImgFlip", 0.5f, key);
//	TRACE("dst_width=%d,dst_height=%d\n", width, height);
//#endif
//    return true;
//}

//bool CImgManipulator::ImgCrop(int width, int height, int bytesPerPixel, unsigned char* src, 
//    int p0x, int p0y, int p1x, int p1y, unsigned char* dst)
//{
//    std::lock_guard<std::mutex> lock(m_mutex);
//
//    int ret = eys::img_alter::Img_Crop_CV(width, height, p0x, p0y, p1x, p1y, 
//        bytesPerPixel, src, dst);
//    if (ret != 1)
//    {
//        OutputDebugString((std::wstring(L"eys::img_alter::Img_Crop_CV failed. (error code: ") + std::to_wstring(ret) + L")").c_str());
//        return false;
//    }
//
//	/*
//	// GrayJao: sample for memcpy 
//	int dataSize = 0;
//	int copyOnceDataLength = (p1x - p0x + 1) * bytesPerPixel;
//
//	for (int i = 0; i < height; i++)
//	{
//		if (i >= p0y && i <= p1y) {
//			int start = (i * width + p0x) * bytesPerPixel;
//			memcpy(&dst[dataSize], &src[start], copyOnceDataLength);
//			dataSize += copyOnceDataLength;
//		}
//	}*/
//	
//    return true;
//}

//bool CImgManipulator::ImgHalfSize(POINT& imgRes, int bytesPerPixel, unsigned long int& imgSize, unsigned char* src, unsigned char* dst)
//{
//    std::lock_guard<std::mutex> lock(m_mutex);
//
//    int w = (bytesPerPixel == 1 ? imgRes.x : imgRes.x/2);
//    int ret = eys::img_alter::Img_Resize_CV(imgRes.x, imgRes.y, w, imgRes.y/2, bytesPerPixel, src, dst);
//    if (ret != 1)
//    {
//        OutputDebugString((std::wstring(L"eys::img_alter::Img_Resize_CV failed. (error code: ") + std::to_wstring(ret) + L")").c_str());
//        return false;
//    }
//    imgRes.x = w;
//    imgRes.y /= 2;
//    imgSize = imgRes.x * imgRes.y * bytesPerPixel;
//	
//#ifdef debug_ImgResizeHalf
//	int key;
//	eys::img_io::Img_Show_CV(imgRes.x, imgRes.y, dst, "ImgHalfSize", 0.5f, key);
//	TRACE("dst_width=%d,dst_height=%d\n", imgRes.x / 2, imgRes.y / 2);
//#endif
//    return true;
//}

//bool CImgManipulator::ImgResizeHalf(POINT& imgRes, int bytesPerPixel, unsigned long int& imgSize, unsigned char* src, unsigned char* dst)
//{
//	std::lock_guard<std::mutex> lock(m_mutex);
//
//	CPoint ResizeRes;
//	if (bytesPerPixel == 1)
//		ResizeRes = CPoint(imgRes.x, imgRes.y / 2);
//	else
//		ResizeRes = CPoint(imgRes.x / 2, imgRes.y / 2);
//	imgSize = ResizeRes.x * ResizeRes.y * bytesPerPixel;
//	std::vector<unsigned char> _Buf;
//	_Buf.resize(imgSize);
//
//	unsigned int dstSize = 0;
//	if (bytesPerPixel == 3)
//	{
//		/*
//		"COLOR_RGB24", bytesPerPixel = 3;
//		For the image type RGB24, the one pixel is the three bytes.
//		Get 2 pixel every times, but actually RGB24 just need 1 pixels .
//		Because The one  Pixel of the half size image is equal to the two Pixels of the full size image.
//		*/
//		for (int i = 0; i < imgRes.y; i += 2)
//		{
//			for (int j = 0; j < imgRes.x; j += 2)
//			{
//				memcpy(&_Buf[0] + dstSize, &(src[(i*imgRes.x + j) * bytesPerPixel]), bytesPerPixel);
//				dstSize += bytesPerPixel;
//			}
//		}
//	}
//	else if (bytesPerPixel == 2)
//	{
//		/*
//		"DEPTH_8BITS", bytesPerPixel = 1;
//		"COLOR_YUY2 \ DEPTH_8BITS_0x80 \ DEPTH_11BITS \ DEPTH_14BITS", bytesPerPixel == 2;
//		For the image type YUYV, the 1 pixel is the 2 bytes. 
//		
//		Get 4 pixels every times, but actually YUYV just need 2 pixels .
//		The 2 Pixels of the half size image is equal to the 4 Pixels of the full size image.
//		*/
//
//		for (int i = 0; i < imgRes.y; i += 2)
//		{
//			for (int j = 0; j < imgRes.x; j += 4)
//			{
//				memcpy(&_Buf[0] + dstSize, &(src[(i*imgRes.x + j) * 2]), bytesPerPixel * 2);
//				dstSize += bytesPerPixel * 2;
//			}
//		}
//	}
//	else if (bytesPerPixel == 1)
//	{
//		/*
//		"DEPTH_8BITS", bytesPerPixel = 1;
//		*/
//		for (int i = 0; i < imgRes.y; i += 2)
//		{
//			for (int j = 0; j < imgRes.x; j += 2)
//			{
//				memcpy(&_Buf[0] + dstSize, &(src[(i*imgRes.x + j) * 2]), bytesPerPixel * 2);
//				dstSize += bytesPerPixel * 2;
//			}
//		}
//	}
//
//	std::copy(_Buf.begin(), _Buf.end(), dst);
//	imgRes.x = ResizeRes.x;
//	imgRes.y = ResizeRes.y;
//
//#ifdef debug_ImgResizeHalf
//	int key;
//	if (bytesPerPixel == 3) {			//COLOR_RGB 
//		eys::img_io::Img_Show_CV(imgRes.x / 2, imgRes.y / 2, dst, "Color_Resize_Half_Img", 0.5f, key);
//	}
//	else if (bytesPerPixel == 2 || bytesPerPixel == 1) {		//Depth_YUV to Depth_RGB
//		unsigned char *img_dst = new unsigned char[imgRes.x / 2 * imgRes.y / 2 * 3];
//		eys::img_format::YUV_422_To_RGB_CV(imgRes.x/2, imgRes.y/2, dst, img_dst);
//		eys::img_io::Img_Show_CV(imgRes.x/2, imgRes.y/2, img_dst, "Depth_Img_Crop_CVResize_Half_Img", 0.5f, key);
//		free(img_dst);
//	}
//	TRACE("dst_width=%d,dst_height=%d\n", imgRes.x/2, imgRes.y/2);
//#endif
//	return true;
//}
//
//long Get_JpegSize(unsigned char* src, long nSize)
//{
//	long i;
//	for (i = 1024; i < nSize; i++)
//	{
//		if ((src[i] == 0xFF) && (src[i + 1] == 0xD9)) return i + 2;
//	}
//	return -1;
//}

//bool CImgManipulator::ImgMjpegToRgb24(POINT& imgRes, unsigned char* src, unsigned char* dst)
//{
//	long int nImgSize = -1;
//	{
//		std::lock_guard<std::mutex> lock(m_mutex);
//		nImgSize = Get_JpegSize(src, imgRes.x*imgRes.y * 2);
//	}
//
//	if (nImgSize != -1)
//	{
//		unsigned char *pTempImgBuf = (unsigned char*)calloc(nImgSize, sizeof(unsigned char));
//
//		memcpy(pTempImgBuf, src, nImgSize);
//		
//		struct jpeg_decompress_struct cinfo;
//		struct my_error_mgr jerr;
//
//		int rc;
//		int row_stride, width, height, pixel_size;
//
//		cinfo.err = jpeg_std_error(&jerr.pub);
//		jerr.pub.error_exit = my_error_exit;
//
//		if (setjmp(jerr.setjmp_buffer)) {
//			jpeg_destroy_decompress(&cinfo);
//			free(pTempImgBuf);
//			pTempImgBuf = NULL;
//			return false;
//		}
//
//		jpeg_create_decompress(&cinfo);
//		jpeg_mem_src(&cinfo, pTempImgBuf, nImgSize);
//
//		rc = jpeg_read_header(&cinfo, TRUE);
//
//		if (rc != 1) {
//			OutputDebugString(_T("File does not seem to be a normal JPEG !!\r\n"));
//			free(pTempImgBuf);
//			pTempImgBuf = NULL;
//			return false;
//		}
//
//		cinfo.out_color_space = JCS_EXT_BGR;
//		cinfo.dct_method = JDCT_IFAST;
//
//		jpeg_start_decompress(&cinfo);
//
//		width = cinfo.output_width;
//		height = cinfo.output_height;
//		pixel_size = cinfo.output_components;
//
//		row_stride = width * pixel_size;
//
//		while (cinfo.output_scanline < cinfo.output_height) {
//			unsigned char *buffer_array[1];
//			buffer_array[0] = dst + (imgRes.x * imgRes.y * 3) - (cinfo.output_scanline + 1) * row_stride;
//			jpeg_read_scanlines(&cinfo, buffer_array, 1);
//		}
//
//		jpeg_finish_decompress(&cinfo);
//		jpeg_destroy_decompress(&cinfo);
//
//		free(pTempImgBuf);
//		pTempImgBuf = NULL;
//
//#ifdef debug_ImgMjpegToRgb24
//		int key;
//		eys::img_io::Img_Show_CV(imgRes.x, imgRes.y, dst, "Color_Img_Mjpeg_To_Rgb24", 0.5f, key);
//#endif
//
//		return true;
//	}
//	return false;
//}
