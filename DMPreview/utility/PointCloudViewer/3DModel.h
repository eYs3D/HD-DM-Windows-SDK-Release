#ifndef __3DMODEL_H__
#define __3DMODEL_H__

#include "BoundingBox.h"
#include "GLSLProgram.h"
#include "rply.h"

struct Vertex
{
	float x;
	float y;
	float z;
};
struct Color
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
};
struct Mesh
{
	unsigned int id0;
	unsigned int id1;
	unsigned int id2;
};

///
/// Class C3DModel.
///
/// A class for loading and displaying and object using Open Asset Import Librery
///
///
class C3DModel
{
private:
	CBoundingBox m_bbox;
	static std::vector<Vertex> m_vVertex;
	static std::vector<Mesh> m_vMesh;
	unsigned int m_uVBO;
	unsigned int m_uVBOIndex;
	int m_iNPoints;
	int m_iNTriangles;
	GLuint m_uVAO;		//for the model
	std::mutex m_dataMutex;
	

	static Color m_tempColor;
	static Vertex m_tempVertex;
	static Mesh m_tempMesh;
	static int m_iTempIndexVertex;
	static int m_iTempIndexColor;

	//
	static std::vector<Color> m_vColor;
	unsigned int m_uVBOColor;

	volatile bool m_isDataUpdate;
protected:
	static int vertex_cb(p_ply_argument argument);
	static int color_cb(p_ply_argument argument);
	static int face_cb(p_ply_argument argument);
	static std::vector<Vertex> Helper(){ std::vector<Vertex> a; a.reserve(1); return a; }

	//
public:
	


	C3DModel();
	~C3DModel();

	void genGLBuffer();

	///Method to load an 3Dmodel
	bool load(const std::string & sFilename);

	///Method to load an 3Dmodel
	bool updateData(std::vector<float>& cloudPoints, std::vector<unsigned char>& colorBuf);

	//
	void updateBuffer();
	//delete all buffers
	void deleteBuffers();

	///Draw the object using the VAO
	void drawObject();

	bool isDataUpdate();

	///Get the center of the object
	inline glm::vec3 getCenter(){return m_bbox.getCenter();}

	///Get the lenght of the diagonal of bounding box
	inline float getDiagonal(){return m_bbox.getDiagonal();}

	
};
#endif
