#include "stdafx.h"
#include "PointCloudViewer.h"

#include "BasicShader.h" 
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glew32s.lib")

using namespace std;

///< Only wrapping the glfw functions
float CPointCloudViewer::DOUBLE_CLICK_INTERVAL = 0.5f;

CPointCloudViewer::CPointCloudViewer(MessageCallbackFn messageCallbackFn, void* pParent) : m_models( new C3DModel() ) {
	m_pParent = pParent;
	m_messageCallbackFn = messageCallbackFn;

	m_glfwWindow = nullptr;
    m_bThreadRun = TRUE;
	/*init viewer mvp*/
	mScroll_y = m_defaultZoom;
	mScaleMatrix = glm::scale(glm::mat4(), glm::vec3(m_factorScale, m_factorScale, m_factorScale));

	m_renderThread = CreateThread( NULL, NULL, RenderThreadFn, this, NULL, NULL );
}


///
/// Init all data and variables.
/// @return true if everything is ok, false otherwise
///
bool CPointCloudViewer::initialize()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonOffset(1, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glewExperimental = GL_TRUE;
	m_bLeftButton = m_bFlag = false;
	if (glewInit() != GLEW_OK)
	{
		TRACE("- glew Init failed \n");
		return false;
	}
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	//load the shaders
	m_program.loadShaderFromBuffer(vertexSource, CGLSLProgram::VERTEX);
	m_program.loadShaderFromBuffer(fragmentSource, CGLSLProgram::FRAGMENT);
	m_program.create_link();
	m_program.enable();
	m_program.addAttribute("vVertex");
	m_program.addUniform("mMVP");
	m_program.addUniform("vec4Color");
	m_program.disable();
	
	//m_models.push_back(new C3DModel());
	m_models->genGLBuffer();
	m_vec4ColorAB = glm::vec4(1, 1, 1 , 0.3);
	m_vec4ColorC = glm::vec4(0.1, 0.01, 0.6, 1.0);
	return true;
}

DWORD CPointCloudViewer::RenderThreadFn( void* pvoid )
{
    CPointCloudViewer* pThis = ( CPointCloudViewer* )pvoid;
	/*Create window*/
	glfwSetErrorCallback(CPointCloudViewer::errorCB);
	if (!glfwInit())	exit(EXIT_FAILURE);

	pThis->m_glfwWindow = glfwCreateWindow(pThis->WINDOW_WIDTH, pThis->WINDOW_HEIGHT, pThis->strNameWindow.c_str(), NULL, NULL);
	if (!pThis->m_glfwWindow)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	{
		std::lock_guard<std::mutex>  lock(pThis->m_glMutex);

		glfwMakeContextCurrent(pThis->m_glfwWindow);
		if (!pThis->initialize()) exit(EXIT_FAILURE);
		glfwSetWindowUserPointer(pThis->m_glfwWindow, pThis);

		resizeCB(pThis->m_glfwWindow, pThis->WINDOW_WIDTH, pThis->WINDOW_HEIGHT);	//just the 1st time

																// set callback
		glfwSetKeyCallback(pThis->m_glfwWindow, keyboardCB);
		glfwSetWindowSizeCallback(pThis->m_glfwWindow, resizeCB);
		glfwSetMouseButtonCallback(pThis->m_glfwWindow, onMouseDown);
		glfwSetCursorPosCallback(pThis->m_glfwWindow, onMouseMove);
		glfwSetScrollCallback(pThis->m_glfwWindow, scroll_callback);

	}
	glfwMakeContextCurrent(NULL);

	//render thread	
	glfwMakeContextCurrent(pThis->m_glfwWindow);
	//int renderCount = 0;
	//float t_updateBuffer = 0;
	//float t_draw = 0;
	float t_frameUpdate = 0;
	//int frameUpdateCount = 0;
    //const int frameUpdateCountThres = 10;

	t_frameUpdate = clock();
	char title[256];		

	while ( pThis->m_bThreadRun )
	{
		if (glfwWindowShouldClose(pThis->m_glfwWindow))
			break;
		
		//clock_t t;

		//t = clock();
		pThis->draw();
		//t_draw += (float)(clock() - t) / CLOCKS_PER_SEC;

		glfwPollEvents();	//or glfwWaitEvents()
		//t = clock();
		//for (int i = 0; i < pThis->m_models.size(); i++) 
		{
            std::lock_guard< std::mutex > lock( pThis->m_mutex );

			if (pThis->m_models->isDataUpdate()) {	//#6258 Entry-Point;

				pThis->m_models->updateBuffer();
				
				/*FPS*/
				if (pThis->m_frameUpdateCount >20) {					
					t_frameUpdate = clock() - t_frameUpdate;
					sprintf(title, "%s  - [FPS: %3.2f]", "Point Cloud Viewer", 1.0f/ ( (t_frameUpdate / CLOCKS_PER_SEC) / pThis->m_frameUpdateCount ) );
					glfwSetWindowTitle(pThis->m_glfwWindow, title);
                    t_frameUpdate = clock();
					pThis->m_frameUpdateCount = 0;
				}
			}
            else std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		//t_updateBuffer += (float)(clock() - t) / CLOCKS_PER_SEC;
		

		//if (renderCount % 100== 0) {
		//	TRACE("renderCount%d  t_draw:%f t_updateBuffer:%f\n", renderCount, t_draw, t_updateBuffer);
		//	t_draw = 0;
		//	t_updateBuffer = 0;
		//}
		//renderCount++;
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	glfwMakeContextCurrent(NULL);
	
	glfwMakeContextCurrent(pThis->m_glfwWindow);

	pThis->destroy();

	glfwMakeContextCurrent(NULL);

    return NULL;
}


void CPointCloudViewer::updateModel(int index,std::vector<float>& cloudPoints, std::vector<unsigned char>& colorBuf)
{
	if (!m_models) return;

    std::lock_guard< std::mutex > lock( m_mutex );
    m_frameUpdateCount++;

	m_models->updateData(cloudPoints,colorBuf);
}

///< Callback function used by GLFW to capture some possible error.
void CPointCloudViewer::errorCB(int error, const char* description)
{
	TRACE(description );
}

void CPointCloudViewer::onMouseMove(GLFWwindow *window, double xpos, double ypos)
{		
	CPointCloudViewer * this_viewer = static_cast<CPointCloudViewer*>(glfwGetWindowUserPointer(window));
	if (this_viewer->m_bLeftButton)
	{
		if (!this_viewer->m_bFlag)
		{
			this_viewer->m_bFlag = true;
			this_viewer->m_arcball.OnMouseDown(glm::ivec2(xpos, ypos));
		}
		else
		{
			this_viewer->m_MousePoint.x = xpos;
			this_viewer->m_MousePoint.y = ypos;
			this_viewer->m_arcball.OnMouseMove(this_viewer->m_MousePoint, ROTATE);
		}
	}
}

void CPointCloudViewer::onMouseDown(GLFWwindow* window, int button, int action, int mods)
{
	CPointCloudViewer * this_viewer = static_cast<CPointCloudViewer*>(glfwGetWindowUserPointer(window));
	if (action == GLFW_PRESS)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			this_viewer->m_bLeftButton = true;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			this_viewer->m_bLeftButton = false;
			this_viewer->m_bFlag = false;
			this_viewer->m_arcball.OnMouseUp(this_viewer->m_MousePoint);

			/*Check if it is double click*/
			float t = (float)(clock() - this_viewer->m_doubleClickTimeStamp )/ CLOCKS_PER_SEC;
			TRACE("t=%f \n", t);
			if (t < this_viewer->DOUBLE_CLICK_INTERVAL) {
				this_viewer->m_arcball.Reset();
				this_viewer->mScroll_y = this_viewer->m_defaultZoom;
                glPointSize(this_viewer->m_point_size);
			}
			this_viewer->m_doubleClickTimeStamp = clock();
		}
	}
}

///
/// The keyboard function call back
/// @param window id of the window that received the event
/// @param iKey the key pressed or released
/// @param iScancode the system-specific scancode of the key.
/// @param iAction can be GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
/// @param iMods Bit field describing which modifier keys were held down (Shift, Alt, & so on)
///
void CPointCloudViewer::keyboardCB(GLFWwindow* window, int iKey, int iScancode, int iAction, int iMods)
{	
	if (iAction == GLFW_PRESS)
	{
		CPointCloudViewer * this_viewer = static_cast<CPointCloudViewer*>(glfwGetWindowUserPointer(window));				
		switch (iKey)
		{
		case GLFW_KEY_ESCAPE:
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		}
		if (this_viewer->m_pParent != nullptr && this_viewer->m_messageCallbackFn != nullptr) {
			this_viewer->m_messageCallbackFn(MSG_KEYBOARD, iKey, this_viewer->m_pParent);
		}

	}
}

///< The resizing function
void CPointCloudViewer::resizeCB(GLFWwindow* window, int iWidth, int iHeight)
{
	CPointCloudViewer * this_viewer = static_cast<CPointCloudViewer*>(glfwGetWindowUserPointer(window));
	if (iHeight == 0) iHeight = 1;
	float ratio = iWidth / float(iHeight);
	glViewport(0, 0, iWidth, iHeight);
	this_viewer->mProjMatrix = glm::perspective(this_viewer->m_fAngle,  ratio, this_viewer->m_NCP, this_viewer->m_FCP);
	this_viewer->m_arcball.Resize(iWidth, iHeight);
}
void CPointCloudViewer::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	CPointCloudViewer * this_viewer = static_cast<CPointCloudViewer*>(glfwGetWindowUserPointer(window));
	float factor = 0.5f;
	this_viewer->mScroll_y += yoffset * factor;
	
}
///< The main rendering function.
void CPointCloudViewer::draw()
{		

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
	//float pointSize = (mScroll_y -  m_defaultZoom ) /**1.3f*/ ;
	//if (pointSize < 0) pointSize = 1.0f;
	//glPointSize(pointSize);
    glPointSize(m_point_size);
	m_program.enable();
		glUniformMatrix4fv(m_program.getLocation("mProjection"), 1, GL_FALSE, glm::value_ptr(mProjMatrix));		
		mModelMatrix =  	glm::rotate(glm::mat4(),180.f, glm::vec3(0.0f, 0.0f, 1.0f))*
							glm::rotate(glm::mat4(), 180.f, glm::vec3(0.0f, 1.0f, 0.0f))*
							m_arcball.GetTransformation()*
							glm::translate(glm::mat4(), glm::vec3(0, 0, -400.0)) 													
							;		
		mViewMatrix = glm::translate(glm::mat4(), glm::vec3(0, 0, mScroll_y)) * mScaleMatrix;
		glm::mat4 mvp = mProjMatrix * mViewMatrix * mModelMatrix;
		glUniformMatrix4fv(m_program.getLocation("mMVP"), 1, GL_FALSE, glm::value_ptr(mvp));

		glUniform4fv(m_program.getLocation("vec4Color"), 1, glm::value_ptr(m_vec4ColorC));

		//for (int i = 0; i < m_models.size(); i++) {
			m_models->drawObject();
		//}		
		
	m_program.disable();
	glfwSwapBuffers(m_glfwWindow);	
}


void CPointCloudViewer::setWindowPos(int x, int y) 
{
	glfwSetWindowPos(m_glfwWindow, x, y);
	
}
/// Here all data must be destroyed + glfwTerminate
void CPointCloudViewer::close()
{
    m_bThreadRun = FALSE;

    WaitForSingleObject( m_renderThread, INFINITE );

	if (m_glfwWindow != nullptr) {
		glfwSetWindowShouldClose(m_glfwWindow, GL_TRUE);
	}
}
void CPointCloudViewer::destroy()
{
	glfwMakeContextCurrent( m_glfwWindow);
    m_program.deleteShaders();

	//for (int i = 0; i < m_models.size(); i++) {
		//m_models[i]->deleteBuffers();
        //delete m_models[i];
	//}
    //m_models->clear();
	m_model.deleteBuffers();
	//glfwSetWindowShouldClose(m_glfwWindow, GL_TRUE);
	glfwDestroyWindow(m_glfwWindow);
	glfwTerminate();	
	m_glfwWindow = nullptr;
	m_messageCallbackFn(MSG_CLOSE, 0, m_pParent);
	//glfwWaitEvents();
}


