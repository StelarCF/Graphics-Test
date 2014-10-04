#include "Render.h"
#include <cmath>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <vector>

#if defined(_WIN32)
#include <SDL.h>
#elif defined(__LINUX__)
#include <SDL2/SDL.h>
#include <GL/glew.h>
#define strcpy_s(x,y,z) strcpy(x,z)
#elif defined(__APPLE__)
#error Buy some apples!
#endif

#include "vmath.h"
#include "lodepng.h"
#include <ctime>
using namespace std;

/// NOTES TO SELF
/// Fix time delta [./] Done! See the render loop

GLuint program;
GLuint vertex_array_object;

/// Loads, compiles and attaches a shader from a filename into an OpenGL program
void LCAShader(string fileName, GLuint program, GLenum shaderType) {
	stringstream strStream;
	string source;
	ifstream file(fileName.c_str());
	if(!file.is_open()) {
		printf("Fatal error: Shader source not found");
		throw "Fatal Error: Shader source not found";
	}
	strStream << file.rdbuf();
	source = strStream.str();
	GLuint shader;
	shader = glCreateShader(shaderType);
	char *charSource;
	charSource = new GLchar[source.size() + 1]; // +1 to include the end character \0
	strcpy_s(charSource, source.size() + 1, source.c_str());
	glShaderSource(shader, 1, (const GLchar **) &charSource, NULL);
	glCompileShader(shader);
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success)
		printf("Shader compilation fail: ");
	else printf("Shader compilation success: ");
	GLint maxLength = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	//The maxLength includes the NULL character
	std::vector<char> errorLog(maxLength);
	glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

	printf("%s\n", &errorLog[0]);
	glAttachShader(program, shader);
	glDeleteShader(shader);
}

void checkError() {
	GLenum glErr;
	glErr = glGetError();
	if(glErr != GL_NO_ERROR)
		printf("%s\n", gluErrorString(glErr));
}

GLuint buffer;
float aspect;
vmath::mat4 proj_matrix;
GLenum t_loc, pos_loc, mv_loc, proj_loc;
GLuint tex;
unsigned int w, h;

vmath::mat4 mv_matrix;
float *fv;
vector<unsigned char> tex_data;

void renderInit() {
	program = glCreateProgram();
	LCAShader("vertex.glsl", program, GL_VERTEX_SHADER);
	// Tesselation and Geometry shaders cause program to crash; not really sure why yet; going on (review at later stage)
	//LCAShader("tessctrlshader.glsl", program, GL_TESS_CONTROL_SHADER);
	//LCAShader("tessevalshader.glsl", program, GL_TESS_EVALUATION_SHADER);
	//LCAShader("geometry.glsl", program, GL_GEOMETRY_SHADER);
	LCAShader("fragment.glsl", program, GL_FRAGMENT_SHADER);
	glLinkProgram(program);
	glValidateProgram(program);
	GLint validProgram;
	glGetProgramiv(program, GL_VALIDATE_STATUS, &validProgram);
	if(!validProgram) {
		printf("Failed validation of program: ");
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> errorLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]);
		printf("%s\n", &errorLog[0]);
	}
	glGenVertexArrays(1, &vertex_array_object);
	glBindVertexArray(vertex_array_object);
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 1024 * 1024, NULL, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	// Wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	aspect = 1300 / 900; //@ Fix to something better
	proj_matrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);

	//pos_loc = glGetUniformLocation(program, "position");
	pos_loc = glGetAttribLocation(program, "position");
	mv_loc = glGetUniformLocation(program, "mv_matrix");
	proj_loc = glGetUniformLocation(program, "proj_matrix");

	glEnable(GL_DEPTH_TEST);
	//glEnable(GLU_CULLING);

	glGenTextures(1, &tex);
	lodepng::decode(tex_data, w, h, "RF.png");
	glBindTexture(GL_TEXTURE_2D, tex);
	checkError();
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);
	checkError();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, &tex_data[0]);
	checkError();

	fv = (float *) mv_matrix;
}

float crtime = 0;

static const float data[] = {
	// Face 1 (front)
	-0.1, -0.1, 0.1,
	0.1, 0.1, 0.1,
	0.1, -0.1, 0.1,

	-0.1, -0.1, 0.1,
	0.1, 0.1, 0.1,
	-0.1, 0.1, 0.1,

	// Face 2 (back)

	-0.1, -0.1, -0.1,
	0.1, 0.1, -0.1,
	0.1, -0.1, -0.1,

	-0.1, -0.1, -0.1,
	0.1, 0.1, -0.1,
	-0.1, 0.1, -0.1,

	// Face 3 (right)

	0.1, -0.1, 0.1,
	0.1, 0.1, -0.1,
	0.1, -0.1, -0.1,

	0.1, -0.1, 0.1,
	0.1, 0.1, -0.1,
	0.1, 0.1, 0.1,

	// Face 4 (left)

	-0.1, -0.1, 0.1,
	-0.1, 0.1, -0.1,
	-0.1, -0.1, -0.1,

	-0.1, -0.1, 0.1,
	-0.1, 0.1, -0.1,
	-0.1, 0.1, 0.1,

	// Face 5 (bottom)

	-0.1, -0.1, 0.1,
	0.1, -0.1, -0.1,
	0.1, -0.1, 0.1,

	-0.1, -0.1, 0.1,
	0.1, -0.1, -0.1,
	-0.1, -0.1, -0.1,

	// Face 6 (top)

	-0.1, 0.1, 0.1,
	0.1, 0.1, -0.1,
	0.1, 0.1, 0.1,

	-0.1, 0.1, 0.1,
	0.1, 0.1, -0.1,
	-0.1, 0.1, -0.1
};

void ProcEvent(SDL_Event event) {
}

void render() {
	const GLfloat color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	//crtime += 0.002;
	crtime = 0.8  * ((float)clock() / CLOCKS_PER_SEC);
	float f = (float) crtime  * (float) M_PI * 0.1f;
	GLfloat one = 1.0f;
	mv_matrix =
		vmath::translate(0.0f, 0.0f, -4.0f) *
		vmath::translate(sinf(2.1f * f) * 0.5f, cosf(1.7f * f) * 0.5f, sinf(1.3f * f) * cosf(1.5f * f) * 2.0f) *
		vmath::rotate((float) crtime * 45.0f, 0.0f, 1.0f, 0.0f) *
		vmath::rotate((float) crtime * 81.0f, 1.0f, 0.0f, 0.0f);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), data);
	checkError();
	glClearBufferfv(GL_COLOR, 0, color);
	glClearBufferfv(GL_DEPTH, 0, &one);
	checkError();
	glUseProgram(program);
	checkError();
	GLfloat attrib[] = { crtime, 0.0f };
	glVertexAttrib2fv(1, attrib);
	glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	checkError();

	vmath::mat4 mat;

	glUniformMatrix4fv(mv_loc, 1, GL_FALSE, mv_matrix);
	checkError();
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, proj_matrix);	

	const float cufk = 0.019;

	checkError();
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	for(long i = 0; i < 100; i++) {
		f += 1.0f;
		mv_matrix = //vmath::translate(cufk * i, -cufk * i, cufk * i) * 
			vmath::translate(0.0f, 0.0f, -4.0f) *
			vmath::translate(sinf(f + 2.1f * i) * 0.5f, cosf(1.7f * f) * 0.5f, sinf(1.3f * f) * cosf(1.5f * f) * 2.0f) *
			vmath::rotate((float) (crtime + 0.1f * i) * 45.0f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate((float) (crtime + 0.2f * i) * 81.0f, 1.0f, 0.0f, 0.0f);
		glUniformMatrix4fv(mv_loc, 1, GL_FALSE, fv);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	checkError();
	//glPolygonMode(GL_FRONT_AND_BACK, GL_POLYGON);
}

void renderFin() {
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteProgram(program);
}
