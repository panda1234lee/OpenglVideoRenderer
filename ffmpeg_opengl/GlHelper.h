#pragma once

#include <GL/glew.h>
#include <string>

//#define GLFW_INCLUDE_GL3
//#define GLFW_NO_GLU
//#include <GL/glfw.h>
//
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class GlHelper
{
public:

    GlHelper();
    ~GlHelper();

	int init(int width, int height, const char* vs_path, const char* fs_path);
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
};

