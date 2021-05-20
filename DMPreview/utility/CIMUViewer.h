#pragma once
#include <afxwin.h>
#include "stdafx.h"
#include "GL/glew/include/GL/glew.h"
#include "GL/glfw/include/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLSLProgram.h"
#include "IMUData.h"

struct CVertexData {
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
	float a;
};

class CIMUViewer : public CWnd
{

public:
	CIMUViewer();
	virtual ~CIMUViewer();

	void UpdateData(IMUData *pIMUData);

	void glCreate(CRect rect, CWnd *parent);
	void glInitialize();

	void InitShader();
	void InitVBO();

	void DestroyVBO();

	void PrepareDeviceBoxData();
	void PrepareAxisData();

	void DrawBox();
	void DrawAxis();

	void DrawScene();

	glm::mat4x4 GetRotateMatrix();
public:
	UINT_PTR m_unpTimer;

private:
	CWnd    *hWnd;
	HDC     hdc;
	HGLRC   hrc;
	int     m_nPixelFormat;
	CRect   m_rect;
	CRect   m_oldWindow;
	CRect   m_originalRect;

	unsigned int m_vboAxisX;
	unsigned int m_vboAxisY;
	unsigned int m_vboAxisZ;
	unsigned int m_vboBox;

	CGLSLProgram m_program;

	glm::mat4x4 m_orthoMatrix;

	glm::quat m_quaternion;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDraw(CDC *pDC);
};

