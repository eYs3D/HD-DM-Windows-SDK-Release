#include "CIMUViewer.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glew32s.lib")

#define GLSL(version, kernel) "#version " version "\n" #kernel

const GLchar* imuVvertexSource = GLSL(
"330",
uniform mat4 mvp_matrix;
attribute vec4 a_position;
attribute mediump vec4 a_color;
varying vec4 v_color;
void main()
{
	gl_Position = mvp_matrix * a_position;
	v_color = a_color;
}
);

const GLchar* imuFragmentSource = GLSL(
"330",
varying vec4 v_color;
void main()
{
	gl_FragColor = v_color;
}
);

CIMUViewer::CIMUViewer():
m_vboAxisX(0), m_vboAxisY(0), m_vboAxisZ(0),
m_vboBox(0)
{

}

CIMUViewer::~CIMUViewer()
{
	DestroyVBO();
	m_program.deleteShaders();
}

void CIMUViewer::UpdateData(IMUData *pIMUData)
{
	m_quaternion = glm::quat(pIMUData->_quaternion[0], 
							 pIMUData->_quaternion[1], pIMUData->_quaternion[2], pIMUData->_quaternion[3]);
}

void CIMUViewer::glCreate(CRect rect, CWnd *parent)
{
	CString className = AfxRegisterWndClass(CS_HREDRAW |
		CS_VREDRAW | CS_OWNDC, NULL,
		(HBRUSH)GetStockObject(BLACK_BRUSH), NULL);

	CreateEx(0, className, L"IMUViewer", WS_CHILD | WS_VISIBLE |
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN, rect, parent, 0);

	// Set initial variables' values  
	m_oldWindow = rect;
	m_originalRect = rect;

	hWnd = parent;
}

void CIMUViewer::glInitialize()
{
	// Initial Setup:  
//  
	static PIXELFORMATDESCRIPTOR pfd =
	{
	   sizeof(PIXELFORMATDESCRIPTOR),
	   1,
	   PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
	   PFD_TYPE_RGBA,
	   32,    // bit depth  
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   16,    // z-buffer depth  
	   0, 0, 0, 0, 0, 0, 0,
	};

	// Get device context only once.  
	hdc = GetDC()->m_hDC;

	// Pixel format.  
	m_nPixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, m_nPixelFormat, &pfd);

	// Create the OpenGL Rendering Context.  
	hrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hrc);

	if (glewInit() != GLEW_OK)
	{
		TRACE("- glew Init failed \n");
		return;
	}

	// Basic Setup: 
	InitShader();
	InitVBO();
	PrepareDeviceBoxData();
	PrepareAxisData();

	// Set color to use when clearing the background.  
	COLORREF crBkgnd = ::GetSysColor(COLOR_3DFACE);
	float r = GetRValue(crBkgnd) / 255.0;
	float g = GetGValue(crBkgnd) / 255.0;
	float b = GetBValue(crBkgnd) / 255.0;

	glClearColor(r, g, b, 1.0f);
	glClearDepth(1.0f);

	// Turn on backface culling  
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	// Turn on depth testing  
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Send draw request  
	OnDraw(NULL);
}

void CIMUViewer::InitShader()
{
	m_program.loadShaderFromBuffer(imuVvertexSource, CGLSLProgram::VERTEX);
	m_program.loadShaderFromBuffer(imuFragmentSource, CGLSLProgram::FRAGMENT);
	m_program.create_link();
	m_program.enable();
	m_program.addAttribute("a_position");
	m_program.addAttribute("a_color");
	m_program.addUniform("mvp_matrix");
}

void CIMUViewer::InitVBO()
{
	glGenBuffers(1, &m_vboAxisX);
	glGenBuffers(1, &m_vboAxisY);
	glGenBuffers(1, &m_vboAxisZ);
	glGenBuffers(1, &m_vboBox);
}

void CIMUViewer::DestroyVBO()
{
	if (m_vboAxisX) glDeleteBuffers(1, &m_vboAxisX);
	if (m_vboAxisY) glDeleteBuffers(1, &m_vboAxisY);
	if (m_vboAxisZ) glDeleteBuffers(1, &m_vboAxisZ);
	if (m_vboBox)   glDeleteBuffers(1, &m_vboBox);
}

void CIMUViewer::PrepareDeviceBoxData()
{
	const float fBoxSize = 1.0f;
	const float BOX_ALPHA = 1.0f;
	const float fColorIntensity = 1.0f;

	CVertexData boxData[] = {
		// front
		{ -fBoxSize / 2, -fBoxSize / 2, fBoxSize / 2, 0.0f, fColorIntensity, 0.0f, BOX_ALPHA},
		{ fBoxSize / 2, -fBoxSize / 2, fBoxSize / 2, 0.0f, fColorIntensity, 0.0f, BOX_ALPHA},
		{ fBoxSize / 2, fBoxSize / 2, fBoxSize / 2, 0.0f, fColorIntensity, 0.0f, BOX_ALPHA},
		{ fBoxSize / 2, fBoxSize / 2, fBoxSize / 2, 0.0f, fColorIntensity, 0.0f, BOX_ALPHA},
		{ -fBoxSize / 2, fBoxSize / 2, fBoxSize / 2, 0.0f, fColorIntensity, 0.0f, BOX_ALPHA},
		{ -fBoxSize / 2, -fBoxSize / 2, fBoxSize / 2, 0.0f, fColorIntensity, 0.0f, BOX_ALPHA},

		// bottom
		{ -fBoxSize / 2, -fBoxSize / 2, fBoxSize / 2, 0.0f, fColorIntensity, fColorIntensity, BOX_ALPHA},
		{ -fBoxSize / 2, -fBoxSize / 2, -fBoxSize / 2, 0.0f, fColorIntensity, fColorIntensity, BOX_ALPHA},
		{ fBoxSize / 2, -fBoxSize / 2, -fBoxSize / 2, 0.0f, fColorIntensity, fColorIntensity, BOX_ALPHA},
		{ fBoxSize / 2, -fBoxSize / 2, -fBoxSize / 2, 0.0f, fColorIntensity, fColorIntensity, BOX_ALPHA},
		{ fBoxSize / 2, -fBoxSize / 2, fBoxSize / 2, 0.0f, fColorIntensity, fColorIntensity, BOX_ALPHA},
		{ -fBoxSize / 2, -fBoxSize / 2, fBoxSize / 2, 0.0f, fColorIntensity, fColorIntensity, BOX_ALPHA},

		// left
		{ -fBoxSize / 2, -fBoxSize / 2, fBoxSize / 2, fColorIntensity, 0.0f, fColorIntensity, BOX_ALPHA},
		{ -fBoxSize / 2, fBoxSize / 2, fBoxSize / 2, fColorIntensity, 0.0f, fColorIntensity, BOX_ALPHA},
		{ -fBoxSize / 2, fBoxSize / 2, -fBoxSize / 2, fColorIntensity, 0.0f, fColorIntensity, BOX_ALPHA},
		{ -fBoxSize / 2, fBoxSize / 2, -fBoxSize / 2, fColorIntensity, 0.0f, fColorIntensity, BOX_ALPHA},
		{ -fBoxSize / 2, -fBoxSize / 2, -fBoxSize / 2, fColorIntensity, 0.0f, fColorIntensity, BOX_ALPHA},
		{ -fBoxSize / 2, -fBoxSize / 2, fBoxSize / 2, fColorIntensity, 0.0f, fColorIntensity, BOX_ALPHA},

		// right
		{ fBoxSize / 2, fBoxSize / 2, -fBoxSize / 2, fColorIntensity, fColorIntensity, 0.0f, BOX_ALPHA},
		{ fBoxSize / 2, fBoxSize / 2, fBoxSize / 2, fColorIntensity, fColorIntensity, 0.0f, BOX_ALPHA},
		{ fBoxSize / 2, -fBoxSize / 2, fBoxSize / 2, fColorIntensity, fColorIntensity, 0.0f, BOX_ALPHA},
		{ fBoxSize / 2, -fBoxSize / 2, fBoxSize / 2, fColorIntensity, fColorIntensity, 0.0f, BOX_ALPHA},
		{ fBoxSize / 2, -fBoxSize / 2, -fBoxSize / 2, fColorIntensity, fColorIntensity, 0.0f, BOX_ALPHA},
		{ fBoxSize / 2, fBoxSize / 2, -fBoxSize / 2, fColorIntensity, fColorIntensity, 0.0f, BOX_ALPHA},

		// back
		{ fBoxSize / 2, fBoxSize / 2, -fBoxSize / 2, 0.0f, 0.0f, fColorIntensity, BOX_ALPHA},
		{ fBoxSize / 2, -fBoxSize / 2, -fBoxSize / 2, 0.0f, 0.0f, fColorIntensity, BOX_ALPHA},
		{ -fBoxSize / 2, -fBoxSize / 2, -fBoxSize / 2, 0.0f, 0.0f, fColorIntensity, BOX_ALPHA},
		{ -fBoxSize / 2, -fBoxSize / 2, -fBoxSize / 2, 0.0f, 0.0f, fColorIntensity, BOX_ALPHA},
		{ -fBoxSize / 2, fBoxSize / 2, -fBoxSize / 2, 0.0f, 0.0f, fColorIntensity, BOX_ALPHA},
		{ fBoxSize / 2, fBoxSize / 2, -fBoxSize / 2, 0.0f, 0.0f, fColorIntensity, BOX_ALPHA},

		// top
		{ fBoxSize / 2, fBoxSize / 2, -fBoxSize / 2, fColorIntensity, 0.0f, 0.0f, BOX_ALPHA},
		{ -fBoxSize / 2, fBoxSize / 2, -fBoxSize / 2, fColorIntensity, 0.0f, 0.0f, BOX_ALPHA},
		{ -fBoxSize / 2, fBoxSize / 2, fBoxSize / 2, fColorIntensity, 0.0f, 0.0f, BOX_ALPHA},
		{ -fBoxSize / 2, fBoxSize / 2, fBoxSize / 2, fColorIntensity, 0.0f, 0.0f, BOX_ALPHA},
		{ fBoxSize / 2, fBoxSize / 2, fBoxSize / 2, fColorIntensity, 0.0f, 0.0f, BOX_ALPHA},
		{ fBoxSize / 2, fBoxSize / 2, -fBoxSize / 2, fColorIntensity, 0.0f, 0.0f, BOX_ALPHA},
	};

	glBindBuffer(GL_ARRAY_BUFFER, m_vboBox);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxData), &boxData[0], GL_STATIC_DRAW);
}

void CIMUViewer::PrepareAxisData()
{
	const float fAxisLength = 2.0f;
	const float fAxisWidth = 0.12f;
	const float fAxisArrowLength = 0.2f;
	const float fAxisArrowWidth = 0.16f;

	CVertexData axisData[] = {
		//Tail
		{0.0f, -fAxisWidth / 2.0, -fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, -fAxisWidth / 2.0, fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, fAxisWidth / 2.0, -fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, fAxisWidth / 2.0, -fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, -fAxisWidth / 2.0, fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, fAxisWidth / 2.0, fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},

		{0.0f, fAxisWidth / 2.0, fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, fAxisWidth / 2.0, fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, fAxisWidth / 2.0, -fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, fAxisWidth / 2.0, -fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, fAxisWidth / 2.0, -fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, fAxisWidth / 2.0, fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},

		{0.0f, fAxisWidth / 2.0, fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, -fAxisWidth / 2.0, fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, fAxisWidth / 2.0, fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, fAxisWidth / 2.0, fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, -fAxisWidth / 2.0, fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, -fAxisWidth / 2.0, fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},

		{fAxisLength, -fAxisWidth / 2.0, fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, -fAxisWidth / 2.0, fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, -fAxisWidth / 2.0, -fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, -fAxisWidth / 2.0, -fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, -fAxisWidth / 2.0, fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, -fAxisWidth / 2.0, -fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},

		{0.0f, -fAxisWidth / 2.0, -fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, fAxisWidth / 2.0, -fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, -fAxisWidth / 2.0, -fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, -fAxisWidth / 2.0, -fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, fAxisWidth / 2.0, -fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, fAxisWidth / 2.0, -fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},

		{fAxisLength, -fAxisWidth / 2.0, -fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, fAxisWidth / 2.0, -fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, fAxisWidth / 2.0, fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, fAxisWidth / 2.0, fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, -fAxisWidth / 2.0, fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, fAxisWidth / 2.0, -fAxisWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},

		//Arrow
		{fAxisLength, -fAxisArrowWidth / 2.0, -fAxisArrowWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, -fAxisArrowWidth / 2.0, fAxisArrowWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, fAxisArrowWidth / 2.0, -fAxisArrowWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, fAxisArrowWidth / 2.0, -fAxisArrowWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, -fAxisArrowWidth / 2.0, fAxisArrowWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, fAxisArrowWidth / 2.0, fAxisArrowWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},

		{fAxisLength, fAxisArrowWidth / 2.0, fAxisArrowWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength + fAxisArrowLength, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, -fAxisArrowWidth / 2.0, fAxisArrowWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},

		{fAxisLength, fAxisArrowWidth / 2.0, fAxisArrowWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength + fAxisArrowLength, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, fAxisArrowWidth / 2.0, -fAxisArrowWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},

		{fAxisLength, -fAxisArrowWidth / 2.0, -fAxisArrowWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength + fAxisArrowLength, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, fAxisArrowWidth / 2.0, -fAxisArrowWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},

		{fAxisLength, -fAxisArrowWidth / 2.0, -fAxisArrowWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength + fAxisArrowLength, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},
		{fAxisLength, -fAxisArrowWidth / 2.0, fAxisArrowWidth / 2.0, 0.0f, 0.0f, 0.0f, 1.0f}

	};

	for (CVertexData &vertexData : axisData) {
		vertexData.r = 1.0f;
		vertexData.g = 0.0f;
		vertexData.b = 0.0f;
		vertexData.a = 1.0f;
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vboAxisX);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axisData), &axisData[0], GL_STATIC_DRAW);

	for (CVertexData &vertexData : axisData) {
		vertexData.r = 0.0f;
		vertexData.g = 1.0f;
		vertexData.b = 0.0f;
		vertexData.a = 1.0f;
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vboAxisY);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axisData), &axisData[0], GL_STATIC_DRAW);

	for (CVertexData &vertexData : axisData) {
		vertexData.r = 0.0f;
		vertexData.g = 0.0f;
		vertexData.b = 1.0f;
		vertexData.a = 1.0f;
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vboAxisZ);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axisData), &axisData[0], GL_STATIC_DRAW);
}

void CIMUViewer::DrawBox()
{
	int nWitdh = m_originalRect.Width();
	int nHeight = m_originalRect.Height();
	glm::mat4x4 model = glm::scale(glm::mat4(), glm::vec3(nWitdh < nHeight ? nWitdh / 3 : nHeight / 3));
	model = glm::scale(model, glm::vec3(3.75f, 1.25f, 1.0f));
	int offset = 0;
	glUniformMatrix4fv(m_program.getLocation("mvp_matrix"), 1, GL_FALSE, glm::value_ptr(m_orthoMatrix * GetRotateMatrix() * model));
	glBindBuffer(GL_ARRAY_BUFFER, m_vboBox);
	glEnableVertexAttribArray(m_program.getLocation("a_position"));
	glVertexAttribPointer(m_program.getLocation("a_position"), 3, GL_FLOAT, GL_FALSE, sizeof(CVertexData), (void *)offset); 
	offset = sizeof(float) * 3;
	glEnableVertexAttribArray(m_program.getLocation("a_color"));
	glVertexAttribPointer(m_program.getLocation("a_color"), 4, GL_FLOAT, GL_FALSE, sizeof(CVertexData), (void *)offset); 
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void CIMUViewer::DrawAxis()
{
	int nWitdh = m_originalRect.Width();
	int nHeight = m_originalRect.Height();
	glm::mat4x4 model = glm::scale(glm::mat4(), glm::vec3(nWitdh < nHeight ? nWitdh / 3 : nHeight / 3));

	int offset = 0;
	glUniformMatrix4fv(m_program.getLocation("mvp_matrix"), 1, GL_FALSE, glm::value_ptr(m_orthoMatrix * GetRotateMatrix() * model));
	glBindBuffer(GL_ARRAY_BUFFER, m_vboAxisX);
	glEnableVertexAttribArray(m_program.getLocation("a_position"));
	glVertexAttribPointer(m_program.getLocation("a_position"), 3, GL_FLOAT, GL_FALSE, sizeof(CVertexData), (void *)offset);
	offset = sizeof(float) * 3;
	glEnableVertexAttribArray(m_program.getLocation("a_color"));
	glVertexAttribPointer(m_program.getLocation("a_color"), 4, GL_FLOAT, GL_FALSE, sizeof(CVertexData), (void *)offset);
	glDrawArrays(GL_TRIANGLES, 0, 54);

	offset = 0;
	glm::mat4x4 rotateToYAxis = glm::rotate(glm::mat4(), 90.0f, glm::vec3(0.0f, 0.0f, 1.0f));	

	glUniformMatrix4fv(m_program.getLocation("mvp_matrix"), 1, GL_FALSE, glm::value_ptr(m_orthoMatrix * GetRotateMatrix() * rotateToYAxis * model));
	glBindBuffer(GL_ARRAY_BUFFER, m_vboAxisY);
	glEnableVertexAttribArray(m_program.getLocation("a_position"));
	glVertexAttribPointer(m_program.getLocation("a_position"), 3, GL_FLOAT, GL_FALSE, sizeof(CVertexData), (void *)offset); 
	offset = sizeof(float) * 3;
	glEnableVertexAttribArray(m_program.getLocation("a_color"));
	glVertexAttribPointer(m_program.getLocation("a_color"), 4, GL_FLOAT, GL_FALSE, sizeof(CVertexData), (void *)offset); 
	glDrawArrays(GL_TRIANGLES, 0, 54);

	offset = 0;
	glm::mat4x4 rotateToZAxis = glm::rotate(glm::mat4(), -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));	

	glUniformMatrix4fv(m_program.getLocation("mvp_matrix"), 1, GL_FALSE, glm::value_ptr(m_orthoMatrix * GetRotateMatrix() * rotateToZAxis * model));
	glBindBuffer(GL_ARRAY_BUFFER, m_vboAxisZ);
	glEnableVertexAttribArray(m_program.getLocation("a_position"));
	glVertexAttribPointer(m_program.getLocation("a_position"), 3, GL_FLOAT, GL_FALSE, sizeof(CVertexData), (void *)offset);
	offset = sizeof(float) * 3;
	glEnableVertexAttribArray(m_program.getLocation("a_color"));
	glVertexAttribPointer(m_program.getLocation("a_color"), 4, GL_FLOAT, GL_FALSE, sizeof(CVertexData), (void *)offset); 
	glDrawArrays(GL_TRIANGLES, 0, 54);
}

void CIMUViewer::DrawScene()
{
	DrawBox();
	DrawAxis();
}

glm::mat4x4 CIMUViewer::GetRotateMatrix()
{
	glm::mat4x4 rotateMatrix = glm::rotate(glm::mat4(), -90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	return rotateMatrix * glm::toMat4(m_quaternion);
}

BEGIN_MESSAGE_MAP(CIMUViewer, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()


void CIMUViewer::OnPaint()
{
	ValidateRect(NULL);
}


int CIMUViewer::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	glInitialize();

	return 0;
}


void CIMUViewer::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:
	{
		// Clear color and depth buffer bits  
		static std::mutex _mutex;
		std::lock_guard<std::mutex> lock(_mutex);

		wglMakeCurrent(hdc, hrc);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw OpenGL scene  
		DrawScene();  

		// Swap buffers  
		SwapBuffers(hdc);

		break;
	}

	default:
		break;
	}

	CWnd::OnTimer(nIDEvent);
}


void CIMUViewer::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (0 >= cx || 0 >= cy || nType == SIZE_MINIMIZED) return;

	// Map the OpenGL coordinates.  
	glViewport(0, 0, cx, cy);

	int nZPlane = cx < cy ? cx : cy;
	m_orthoMatrix = glm::ortho<float>(-cx, cx, -cy, cy, -nZPlane, nZPlane);
}

void CIMUViewer::OnDraw(CDC *pDC)
{

}
