
const GLchar* vertexSource =
"#version 330 \n"
"uniform mat4 mMVP; "
"layout(location = 0) in vec4 vVertex; "
"layout(location = 1) in vec3 vColor;"
"out vec3 vVertexColor;"
"void main() { "
	"vVertexColor = vColor;"
	"gl_Position = mMVP * vVertex;"
"}";
const GLchar* fragmentSource =
"#version 330 \n"
"in vec3 vVertexColor;"
"out vec4 vFragColor;"
"void main(void){"
	"vFragColor = vec4(vVertexColor.z,vVertexColor.y,vVertexColor.x,1.0);"
"}";


