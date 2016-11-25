#pragma once

#include <GL/glew.h>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class GlHelper
{
public:

	static GlHelper* getInstance()
	{
		if (!s_instance)
		{
			s_instance = new GlHelper();
		}

		return s_instance;
	}

	static void release()
	{
		if(s_instance)
			delete s_instance;
		s_instance = NULL;
	}

	int init(int width, int height, const char* vs_path, const char* fs_path);
	void initVAO();
	void initTexture(int width, int height);
	void initMatrix();
    void drawFrame();
    void clear();

    static std::string textFileRead(const char *file_name);

	// 为当前的纹理传递数据
    static void deliverDataToTexture(int width, int height, unsigned char *data)
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width,
                        height, GL_RGB, GL_UNSIGNED_BYTE,
                        data);
    }

private:
	GlHelper();
	~GlHelper();

	bool buildShader(std::string const &shader_source, GLuint &shader, GLenum type);
	bool buildProgram();

private:
    GLuint m_vao;
    GLuint m_vert_buf;
    GLuint m_elem_buf;
    GLuint m_frame_tex;
    GLuint m_program;
    GLuint m_attribs[2];
    GLuint m_uniforms[2];

    std::string m_vert_shader_source;
    std::string m_frag_shader_source;

	static GlHelper* s_instance;
};

