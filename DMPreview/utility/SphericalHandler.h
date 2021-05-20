#pragma once
#include <vector>


class CSphericalHandler
{
public:
    CSphericalHandler();
    virtual ~CSphericalHandler();
    void SetImageResolution(int width, int height);
    void GetImageResolution(int& width, int& height);
    void DrawImage(HDC targetDC, RECT targetRect, const std::vector<unsigned char>& flatImg, bool spherical360);
    void Home();
    void Pan(float pan);// "< 0" for right
    void Tilt(float tilt);// "< 0" for up
    void Zoom(float zoom);// "< 0" for zoom in
    void PanTilt(float pan, float tilt);
    void GetPosition(float& pan, float& tilt, float& zoom);// pan: 0.0 to 360.0, tilt: -90.0 to 90.0, zoom: 30.0 to 170.0
    void SetPosition(float pan, float tilt, float zoom);
    void SetRotate180(bool rotate);
    bool IsRotate180();

public:
    static const float m_stepPanValue;
    static const float m_stepTiltValue;
    static const float m_stepZoomValue;


private:
    void CheckAndSetOutImgResolution(int outWidth, int outHeight);

private:
    void* m_sphereHandle;
    float m_fieldOfView;
    CSize m_inImgRes;
    CSize m_outImgRes;
};