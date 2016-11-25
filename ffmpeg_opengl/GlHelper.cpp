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
	FRAME_TEX,
	IMAGE_TEX_0,
	IMAGE_TEX_1
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

	loadInitTexture("../Images/V.jpg", width, height, 0);
	loadInitTexture("../Images/beard.jpg", width, height, 1);

	return 0;
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
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,	// data.codec_ctx->width, data.codec_ctx->height
		0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glUniform1i(m_uniforms[FRAME_TEX], 0);	// 0号纹理单元
}

void GlHelper::ratioResize(const cv::Mat& src, cv::Mat& dst, int dst_w, int dst_h, float t )
{
	int w = src.cols;
	int h = src.rows;
	float m = float(w) / h;

	//float t = 1.;											// 必须大于等于1，缩放系数
	if (t < 1.)
	{
		std::cout << "Bad t value !" << std::endl;
		return;
	}

	float mm = float(dst_w) / dst_h;

	dst = cv::Mat(cv::Size(dst_w, dst_h), CV_8UC3, cv::Scalar(0));

	// 如果原图小于指定尺寸
	if (dst_w > w && dst_h > h)
	{
		//std::cout << " dst_w > w and dst_h > h !" << std::endl;
		// 直接扩展黑边即可
		int delta_x = .5 * (dst_w - w);
		int delta_y = .5 * (dst_h - h);
		// 防止 dst_w - w、dst_h - h 为奇数
		copyMakeBorder(src, dst, dst_h - h - delta_y, delta_y, dst_w - w - delta_x, delta_x,
			cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

		//imshow("Result", extend);
		//cv::waitKey(0);

		return;
	}

	float len = t * std::max(w, h);					// 缩放前的最大边
	float len_ = std::min(dst_w, dst_h);		// 缩放后的最小边

	if (m > 1.f && mm > 1.f)						// 缩放前和缩放后皆为横图
	{
		float n = float(dst_w) / w;					// 确保缩放后的大小能够容纳所有像素
		if (dst_h >= h * n)
		{
			len_ = std::max(dst_w, dst_h);		// 缩放后的最大边
		}
	}
	else 	if (m < 1.f && mm < 1.f)				// 缩放前和缩放后皆为竖图
	{
		float n = float(dst_w) / w;					// 确保缩放后的大小能够容纳所有像素
		if (dst_h >= h * n)
		{
			len = t * std::min(w, h);					// 缩放前的最小边
		}
	}

	float ratio = len_ / len;							// 得到缩放比例
	//std::cout << "Scale ratio: " << ratio << std::endl;

	int w_ = ratio * w;									// 缩放后的实际宽高
	int h_ = ratio * h;
	int delta_x = (dst_w - w_) / (2 * ratio);	// 计算缩放后的尺寸与目标尺寸的差值，用该值的一半换算为缩放前的值
	int delta_y = (dst_h - h_) / (2 * ratio);	// 该值用于平移图像至中心

	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			float u = (i + delta_x) / len;
			float v = (j + delta_y) / len;

			int x = u * len_;
			int y = v * len_;

			dst.at<cv::Vec3b>(y, x)[0] = src.at<cv::Vec3b>(j, i)[0];
			dst.at<cv::Vec3b>(y, x)[1] = src.at<cv::Vec3b>(j, i)[1];
			dst.at<cv::Vec3b>(y, x)[2] = src.at<cv::Vec3b>(j, i)[2];
		}
	}

	//imshow("Result", dst);
	//cv::waitKey(0);
}

void  GlHelper::loadInitTexture(const char *file_path, int width, int height, int index)
{
	cv::Mat img = cv::imread(file_path, 1);

	cv::Mat scale, rgb;
	//flip(img, img, 1);	// 如果用的gl_FragCoord.xy，纹理 需要Y轴翻转

	//resize(img, scale, cv::Size(width, height));
	ratioResize(img, scale, width, height);
	//imshow("Result", scale);
	//cv::waitKey(0);

	cvtColor(scale, rgb, CV_BGR2RGB);

	glGenTextures(1, &m_image_tex[index]);
	glBindTexture(GL_TEXTURE_2D, m_image_tex[index]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// 耗时
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glBindTexture(GL_TEXTURE_2D, 0);

	glUniform1i(m_uniforms[IMAGE_TEX_0 + index], index+1);	// 1号纹理单元

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
	m_uniforms[IMAGE_TEX_0] = glGetUniformLocation(m_program, "imageTex_0");
	m_uniforms[IMAGE_TEX_1] = glGetUniformLocation(m_program, "imageTex_1");

	m_attribs[VERTICES] = glGetAttribLocation(m_program, "vertex");
	m_attribs[TEX_COORDS] = glGetAttribLocation(m_program, "texCoord0");

	return true;
}

// draw frame in opengl context
void GlHelper::drawFrame()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_frame_tex);
	//glUniform1i(m_uniforms[FRAME_TEX], 0);	

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_image_tex[0]);
	//glUniform1i(m_uniforms[IMAGE_TEX], 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_image_tex[1]);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

// clean up the app data structure
void GlHelper::clear()
{
	//glDeleteVertexArrays(1, &data->m_vao);
	glDeleteBuffers(1, &m_vert_buf);
	glDeleteBuffers(1, &m_elem_buf);
	glDeleteTextures(1, &m_frame_tex);
	glDeleteTextures(2, m_image_tex);
}
