#include "stdafx.h"
#include "GlHelper.h"

#include <iostream>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

GlHelper* GlHelper::s_instance = NULL;

// attribute indices
enum
{
	VERTICES = 0,
	TEX_COORDS
};

// uniform indices
enum
{
	MVP_MATRIX = 0,
	FRAME_TEX
};

GlHelper::GlHelper()
{
}

GlHelper::~GlHelper()
{
	clear();
}

int GlHelper::init(int width , int height, const char* vs_path, const char* fs_path)
{
	// initialize opengl
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_TEXTURE_2D);

	m_vert_shader_source = textFileRead(vs_path);
	m_frag_shader_source = textFileRead(fs_path);
	//std::cout << vs << std::endl;
	//std::cout << fs << std::endl;

	// initialize shaders
	if (!buildProgram())
	{
		std::cout << "failed to initialize shaders" << std::endl;
		clear();
		return -1;
	}
	glUseProgram(m_program);

	initVAO();
	initTexture(width, height);
	initMatrix();

}

void GlHelper::initVAO()
{
	// initialize renderable
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// Quad
	glGenBuffers(1, &m_vert_buf);
	glBindBuffer(GL_ARRAY_BUFFER, m_vert_buf);
	float quad[20] =
	{
		-1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 0.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
	// vertices
	glVertexAttribPointer(m_attribs[VERTICES], 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
		BUFFER_OFFSET(0));
	glEnableVertexAttribArray(m_attribs[VERTICES]);
	// texcoords
	glVertexAttribPointer(m_attribs[TEX_COORDS], 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
		BUFFER_OFFSET(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(m_attribs[TEX_COORDS]);

	// Index
	glGenBuffers(1, &m_elem_buf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elem_buf);
	unsigned char elem[6] =
	{
		0, 1, 2,
		0, 2, 3
	};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elem), elem, GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void GlHelper::initTexture(int width, int height)
{
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &m_frame_tex);
	glBindTexture(GL_TEXTURE_2D, m_frame_tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,	// data.codec_ctx->width, data.codec_ctx->height
		0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glUniform1i(m_uniforms[FRAME_TEX], 0);
}

void GlHelper::initMatrix()
{
	glm::mat4 mvp = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(m_uniforms[MVP_MATRIX], 1, GL_FALSE, glm::value_ptr(mvp));
}

// 读取本地文本
std::string GlHelper::textFileRead(const char *file_name)
{
	char *s = (char *)malloc(8000);
	memset(s, 0, 8000);
	FILE *infile = fopen(file_name, "rb");
	int len = fread(s, 1, 8000, infile);
	fclose(infile);
	s[len] = 0;

	std::string ss = s;

	free(s);

	return ss;
}

bool GlHelper::buildShader(std::string const &shader_source, GLuint &shader, GLenum type)
{
	int size = shader_source.length();

	shader = glCreateShader(type);
	char const *c_shader_source = shader_source.c_str();
	glShaderSource(shader, 1, (GLchar const **)&c_shader_source, &size);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		std::cout << "failed to compile shader" << std::endl;
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char *log = new char[length];
		glGetShaderInfoLog(shader, length, &length, log);
		std::cout << log << std::endl;
		delete[] log;
		return false;
	}

	return true;
}

// initialize shaders
bool GlHelper::buildProgram()
{
	GLuint v_shader, f_shader;
	if (!buildShader(m_vert_shader_source, v_shader, GL_VERTEX_SHADER))
	{
		std::cout << "failed to build vertex shader" << std::endl;
		return false;
	}

	if (!buildShader(m_frag_shader_source, f_shader, GL_FRAGMENT_SHADER))
	{
		std::cout << "failed to build fragment shader" << std::endl;
		return false;
	}

	m_program = glCreateProgram();
	glAttachShader(m_program, v_shader);
	glAttachShader(m_program, f_shader);
	glLinkProgram(m_program);
	GLint status;
	glGetProgramiv(m_program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		std::cout << "failed to link m_program" << std::endl;
		int length;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length);
		char *log = new char[length];
		glGetShaderInfoLog(m_program, length, &length, log);
		std::cout << log << std::endl;
		delete[] log;
		return false;
	}

	m_uniforms[MVP_MATRIX] = glGetUniformLocation(m_program, "mvpMatrix");
	m_uniforms[FRAME_TEX] = glGetUniformLocation(m_program, "frameTex");

	m_attribs[VERTICES] = glGetAttribLocation(m_program, "vertex");
	m_attribs[TEX_COORDS] = glGetAttribLocation(m_program, "texCoord0");

	return true;
}

// draw frame in opengl context
void GlHelper::drawFrame()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, m_frame_tex);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
	glBindVertexArray(0);
}

// clean up the app data structure
void GlHelper::clear()
{
	//glDeleteVertexArrays(1, &data->m_vao);
	glDeleteBuffers(1, &m_vert_buf);
	glDeleteBuffers(1, &m_elem_buf);
	glDeleteTextures(1, &m_frame_tex);
}
