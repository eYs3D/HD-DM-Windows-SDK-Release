#include "stdafx.h"
#include "GL/glew/include/GL/glew.h"
#include "3DModel.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "OGLBasic.h"

//#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define BUFFER_OFFSET(i) (reinterpret_cast<void*>(i))
std::vector<Vertex> C3DModel::m_vVertex(0);
std::vector<Mesh> C3DModel::m_vMesh(0);


Vertex C3DModel::m_tempVertex = { 0, 0, 0 };
Color C3DModel::m_tempColor = { 0, 0, 0 };


int C3DModel::m_iTempIndexVertex = 0;

//
std::vector<Color> C3DModel::m_vColor(0);
Mesh C3DModel::m_tempMesh = { 0, 0, 0 };
int C3DModel::m_iTempIndexColor = 0;
///
/// default constructor
///
C3DModel::C3DModel()
{
	m_uVAO = 0;
	m_uVBOIndex = 0;
	m_uVBO = 0;
	m_iNPoints = 0;	
	m_iTempIndexColor = 0;
	m_vMesh.clear();
	m_vVertex.clear();
	m_vColor.clear();

	//
	m_iTempIndexVertex = 0;
	m_uVBOColor = 0;

	m_isDataUpdate = false;

}

///
/// default destructor
///
C3DModel::~C3DModel()
{
	m_vMesh.clear();
	m_vVertex.clear();
	m_vColor.clear();
	//deleteBuffers();
	//TRACE("model unloaded\n");
}


void C3DModel::genGLBuffer() 
{
	glGenVertexArrays(1, &m_uVAO);
	glGenBuffers(1, &m_uVBO);
	glGenBuffers(1, &m_uVBOColor);
}

void C3DModel::deleteBuffers()
{
    if ( m_uVAO      ) { glDeleteBuffers( 1, &m_uVAO      ); m_uVAO      = NULL; }
	if ( m_uVBO      ) { glDeleteBuffers( 1, &m_uVBO      ); m_uVBO      = NULL; }
    if ( m_uVBOIndex ) { glDeleteBuffers( 1, &m_uVBOIndex ); m_uVBOIndex = NULL; }
    if ( m_uVBOColor ) { glDeleteBuffers( 1, &m_uVBOColor ); m_uVBOColor = NULL; }
}

//vertex callback
int C3DModel::vertex_cb(p_ply_argument argument) {
	long eol;
	ply_get_argument_user_data(argument, NULL, &eol);
	switch (m_iTempIndexVertex)
	{
	case 0:
		m_tempVertex.x = float(ply_get_argument_value(argument));
		m_iTempIndexVertex++;
		break;
	case 1:
		m_tempVertex.y = float(ply_get_argument_value(argument));
		m_iTempIndexVertex++;
		break;
	case 2:
		m_tempVertex.z = float(ply_get_argument_value(argument));
		m_vVertex.push_back(m_tempVertex);
		m_iTempIndexVertex = 0;
		break;

	}
	return 1;
}
//color callback
int C3DModel::color_cb(p_ply_argument argument) {
	long eol;
	ply_get_argument_user_data(argument, NULL, &eol);
	switch (m_iTempIndexColor)
	{
	case 0:
		m_tempColor.r = unsigned char(ply_get_argument_value(argument));
		m_iTempIndexColor++;
		break;
	case 1:
		m_tempColor.g = unsigned char(ply_get_argument_value(argument));
		m_iTempIndexColor++;
		break;
	case 2:
		m_tempColor.b = unsigned char(ply_get_argument_value(argument));
		m_vColor.push_back(m_tempColor);
		m_iTempIndexColor = 0;
		break;

	}
	return 1;
}
//face callback
int C3DModel::face_cb(p_ply_argument argument) {
	long length, value_index;
	ply_get_argument_property(argument, NULL, &length, &value_index);
	switch (value_index) {
	case 0:
		m_tempMesh.id0 = (int)ply_get_argument_value(argument);
		break;
	case 1:
		m_tempMesh.id1 = (int)ply_get_argument_value(argument);
		break;
	case 2:
		m_tempMesh.id2 = (int)ply_get_argument_value(argument);
		m_vMesh.push_back(m_tempMesh);
		break;
	default:
		break;
	}
	return 1;
}


///
/// Function to load a 3D object file
///
/// @param sFilename the filename of the 3d object
///
/// @return true if it is load correctly, false otherwise
///
bool C3DModel::load(const std::string & sFilename)
{
	//TRACE("loading the file %s\n", sFilename.c_str());
	long nvertices, ntriangles;
	p_ply ply = ply_open(sFilename.c_str(), NULL, 0, NULL);
	if (!ply) return false;
	if (!ply_read_header(ply)) return false;
	nvertices = ply_set_read_cb(ply, "vertex", "x", vertex_cb, NULL, 0);
	ply_set_read_cb(ply, "vertex", "y", vertex_cb, NULL, 0);
	ply_set_read_cb(ply, "vertex", "z", vertex_cb, NULL, 1);
	ply_set_read_cb(ply, "vertex", "red", color_cb, NULL, 0);
	ply_set_read_cb(ply, "vertex", "green", color_cb, NULL, 0);
	ply_set_read_cb(ply, "vertex", "blue", color_cb, NULL, 0);
	m_iNTriangles = ply_set_read_cb(ply, "face", "vertex_indices", face_cb, NULL, 0);
	//printf("%ld\n%ld\n", nvertices, ntriangles);
	if (!ply_read(ply)) return false;
	ply_close(ply);

	m_iNPoints = m_vVertex.size();
	m_iNTriangles = m_vMesh.size();

	//creating the VAO for the model
	glGenVertexArrays(1, &m_uVAO);
	glBindVertexArray(m_uVAO);

		//creating the VBO
		
		//glGenBuffers(1, &m_uVBOIndex);
		
		glBindBuffer(GL_ARRAY_BUFFER, m_uVBO);
		glBufferData(GL_ARRAY_BUFFER, m_iNPoints * sizeof(Vertex), &m_vVertex[0], GL_STATIC_DRAW);		

		
		glBindBuffer(GL_ARRAY_BUFFER, m_uVBOColor);
		glBufferData(GL_ARRAY_BUFFER, m_iNPoints * sizeof(Color), &m_vColor[0], GL_STATIC_DRAW);

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uVBOIndex);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_iNTriangles * sizeof(Mesh), &m_vMesh[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, m_uVBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0)); //Vertex

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, m_uVBOColor);
		glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Color), BUFFER_OFFSET(0)); //Color

	glBindVertexArray(0);	//VAO

	m_vVertex.clear();
	m_vMesh.clear();
	m_vColor.clear();

	return true;
}

bool C3DModel::updateData(std::vector<float>& cloudPoints, std::vector<unsigned char>& colorBuf)
{
	std::lock_guard<std::mutex>  lock(m_dataMutex);

	if (m_vVertex.size() != (cloudPoints.size() / 3))
		m_vVertex.resize(cloudPoints.size() / 3 );
	if (m_vColor.size() != (colorBuf.size() / 3))
		m_vColor.resize(colorBuf.size() / 3);
	if (m_vVertex.size() > 0 && m_vColor.size()) {
		memcpy(&m_vVertex[0], &cloudPoints[0], cloudPoints.size() * sizeof(float));
		memcpy(&m_vColor[0], &colorBuf[0], colorBuf.size() * sizeof(unsigned char));
	}
	m_iNPoints = m_vVertex.size();

	//creating the VAO for the model
	//glGenVertexArrays(1, &m_uVAO);
	m_isDataUpdate = true;
	
	return true;
}

void C3DModel::updateBuffer() {

	std::lock_guard<std::mutex>  lock(m_dataMutex);
	glBindVertexArray(m_uVAO);

	if (m_vVertex.size() > 0 && m_vColor.size()) {
		//creating the VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_uVBO);
		glBufferData(GL_ARRAY_BUFFER, m_iNPoints * sizeof(Vertex), &m_vVertex[0], GL_STATIC_DRAW);
		
		glBindBuffer(GL_ARRAY_BUFFER, m_uVBOColor);
		glBufferData(GL_ARRAY_BUFFER, m_iNPoints * sizeof(Color), &m_vColor[0], GL_STATIC_DRAW);

	}
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uVBOIndex);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_iNTriangles * sizeof(Mesh), &m_vMesh[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_uVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0)); //Vertex

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, m_uVBOColor);
	glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Color), BUFFER_OFFSET(0)); //Color

	glBindVertexArray(0);	//VAO

	m_isDataUpdate = false;
}

///
/// Method to draw the object
///
void C3DModel::drawObject()
{
	glBindVertexArray(m_uVAO);
	
	glDrawArrays(GL_POINTS,0,m_iNPoints  /**3*/ ); // 3 vertex per point
	//glDrawElements(GL_POINTS, m_iNTriangles * 3*3, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
	glBindVertexArray(0);
}

bool C3DModel::isDataUpdate()
{

	return m_isDataUpdate;
}
