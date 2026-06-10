#pragma once
#define GLEW_STATIC
#include "stdafx.h"
#include "GL/glew/include/GL/glew.h"
#include "GL/glfw/include/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLSLProgram.h"
#include "3DModel.h"
#include "ArcBall.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;

///< Only wrapping the glfw functionjs
class CPointCloudViewer
{
public:
	enum MessageType {
		MSG_KEYBOARD,
		MSG_CLOSE
	};

	typedef void(*MessageCallbackFn)(MessageType msg, int value, void* pParent);
private:
	static float DOUBLE_CLICK_INTERVAL  ;

	GLFWwindow* m_glfwWindow;
	unsigned int WINDOW_WIDTH = 960;
	unsigned int WINDOW_HEIGHT = 540;
	float m_NCP = 0.01f;
	float m_FCP = 100.f;
	float m_fAngle = 80.f;
	float m_factorScale = 0.01;
	float m_defaultZoom = -3.0f;
    volatile float m_point_size = 1.0f;
    volatile int m_frameUpdateCount = 0;

	string strNameWindow = "Point Cloud Viewer";
	glm::vec4 m_vec4ColorAB;
	glm::vec4 BLACK = glm::vec4(0, 0, 0, 1);
	glm::vec4 m_vec4ColorC;
	C3DModel m_model;
	std::unique_ptr< C3DModel > m_models;
	CArcBall m_arcball;
	bool m_bLeftButton;
	bool m_bFlag;
	glm::ivec2 m_MousePoint = glm::ivec2(0,0);
	CGLSLProgram m_program;
	glm::mat4x4 mProjMatrix, mViewMatrix,mModelMatrix;
	glm::mat4x4 mScaleMatrix;

    std::mutex m_mutex;

	float mScroll_y ;

	MessageCallbackFn m_messageCallbackFn = nullptr;
	void* m_pParent = nullptr;
	
	//std::thread *m_renderThread;
    HANDLE m_renderThread;
	std::mutex m_glMutex;	

	clock_t m_doubleClickTimeStamp = 0 ;
    volatile BOOL m_bThreadRun;
	/**/
    static DWORD __stdcall RenderThreadFn( void* pvoid );
public:

	CPointCloudViewer(MessageCallbackFn messageCallbackFn, void* pParent);

	bool initialize();	

	void updateModel(int index, std::vector<float>& cloudPoints, std::vector<unsigned char>& colorBuf);	

	///< Callback function used by GLFW to capture some possible error.
	static void errorCB(int error, const char* description);

	static void onMouseMove(GLFWwindow *window, double xpos, double ypos);

	static void onMouseDown(GLFWwindow* window, int button, int action, int mods);

	///
	/// The keyboard function call back
	/// @param window id of the window that received the event
	/// @param iKey the key pressed or released
	/// @param iScancode the system-specific scancode of the key.
	/// @param iAction can be GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
	/// @param iMods Bit field describing which modifier keys were held down (Shift, Alt, & so on)
	///
	static void keyboardCB(GLFWwindow* window, int iKey, int iScancode, int iAction, int iMods);

	///< The resizing function
	static void resizeCB(GLFWwindow* window, int iWidth, int iHeight);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	///< The main rendering function.
	void draw();

	void setWindowPos(int x,int y);
	/// Here all data must be destroyed + glfwTerminate
	void close();
	void destroy();
    inline BOOL IsWindow() { return m_glfwWindow != NULL; }
    void SetPointSize( const float size )
    {
        m_point_size = size;
    }
};
