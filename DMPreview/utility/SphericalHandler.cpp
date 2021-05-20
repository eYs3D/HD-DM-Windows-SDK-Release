#include "stdafx.h"
#include "SphericalHandler.h"
#include "eYsSphere.h"
#include <sstream>


#pragma comment(lib, "eYsSphere.lib")

const float CSphericalHandler::m_stepPanValue = 5.0;
const float CSphericalHandler::m_stepTiltValue = 2.5;
const float CSphericalHandler::m_stepZoomValue = 1.0;


CSphericalHandler::CSphericalHandler()
    : m_sphereHandle(nullptr), m_fieldOfView(120.0)
{
    int ret = eSphere_Init(&m_sphereHandle, TRUE);
    if (ret != 0)
    {
        std::ostringstream msg;
        msg << "eSphere_Init failed (error code: " << ret << ")" << std::endl;
        OutputDebugString(msg.str().c_str());
    }
}

CSphericalHandler::~CSphericalHandler()
{
    eSphere_Release(m_sphereHandle);
}

void CSphericalHandler::CheckAndSetOutImgResolution(int outWidth, int outHeight)
{
    outWidth = (outWidth >> 2) << 2;// should be multiple of 4
    outHeight = (outHeight >> 2) << 2;// should be multiple of 4
    if (m_outImgRes.cx != outWidth || m_outImgRes.cy != outHeight)
    {
        if (eSphere_SetOutput(m_sphereHandle, outWidth, outHeight, m_fieldOfView) != ESPHERE_OK)
        {
            OutputDebugString("eSphere_SetOutput failed.");
            return;
        }

        m_outImgRes.cx = outWidth;
        m_outImgRes.cy = outHeight;
    }
}

void CSphericalHandler::SetImageResolution(int width, int height)
{
    CheckAndSetOutImgResolution(width, height);

    eSphere_Home(m_sphereHandle);

    m_inImgRes.cx = width;
    m_inImgRes.cy = height;
}

void CSphericalHandler::GetImageResolution(int& width, int& height)
{
    width = m_inImgRes.cx;
    height = m_inImgRes.cy;
}

void CSphericalHandler::DrawImage(HDC targetDC, RECT targetRect, const std::vector<unsigned char>& flatImg, bool spherical360)
{
    CheckAndSetOutImgResolution(targetRect.right - targetRect.left, targetRect.bottom - targetRect.top);

    if (spherical360)
    {
        eSphere_Draw(m_sphereHandle, targetDC, (void*)&flatImg[0], m_inImgRes.cx, m_inImgRes.cy, targetRect);
    }
    else
    {
        eSphere_DrawFlat(m_sphereHandle, targetDC, (void*)&flatImg[0], m_inImgRes.cx, m_inImgRes.cy, targetRect);
    }
}

void CSphericalHandler::Home()
{
    eSphere_Home(m_sphereHandle);
}

void CSphericalHandler::Pan(float pan)
{
    eSphere_StepPanTilt(m_sphereHandle, pan, 0.0);
}

void CSphericalHandler::Tilt(float tilt)
{
    eSphere_StepPanTilt(m_sphereHandle, 0.0, tilt);
}

void CSphericalHandler::PanTilt(float pan, float tilt)
{
    eSphere_StepPanTilt(m_sphereHandle, pan, tilt);
}

void CSphericalHandler::Zoom(float zoom)
{
    eSphere_StepZoom(m_sphereHandle, zoom);
}

void CSphericalHandler::GetPosition(float& pan, float& tilt, float& zoom)
{
    eSphere_GetPos(m_sphereHandle, pan, tilt, zoom);
}

void CSphericalHandler::SetPosition(float pan, float tilt, float zoom)
{
    eSphere_SetPos(m_sphereHandle, pan, tilt, zoom);
}

void CSphericalHandler::SetRotate180(bool rotate)
{
    eSphere_SetReversedOrientation(m_sphereHandle, rotate ? TRUE : FALSE);
}

bool CSphericalHandler::IsRotate180()
{
    return (eSphere_GetReversedOrientation(m_sphereHandle) == TRUE);
}