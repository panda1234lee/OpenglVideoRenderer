#pragma once

#include <GL/glew.h>
#include <string>
#include <opencv2/opencv.hpp>
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
	void ratioResize(const cv::Mat& src, cv::Mat& dst, int dst_w, int dst_h, float t = 1. );
	void loadInitTexture(const char *file_path, int width, int height, int index);
    void drawFrame();
    void clear();

    static std::string textFileRead(const char *file_name);

	// 为当前的纹理传递数据
    void deliverDataToTexture(int width, int height, unsigned char *data)
    {
		glBindTexture(GL_TEXTURE_2D, m_frame_tex);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width,
                        height, GL_RGB, GL_UNSIGNED_BYTE,
                        data);
		glBindTexture(GL_TEXTURE_2D, 0);
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
	GLuint m_image_tex[2];

    GLuint m_program;
    GLuint m_attribs[2];
    GLuint m_uniforms[4];

    std::string m_vert_shader_source;
    std::string m_frag_shader_source;

	static GlHelper* s_instance;
};

