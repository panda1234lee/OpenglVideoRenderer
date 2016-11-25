// ffmpeg_opengl.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "GlutHelper.h"
#include "GlHelper.h"
#include "FFmpegDecoder.h"

//typedef  char  GLchar;

#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char *argv[])
{
	int w = 640, h = 480;
	FFmpegDecoder * decoder = new FFmpegDecoder();
	decoder->init("../video/test.mov", w, h);	// �е�mp4��ʽ��Ī������
	
	//w = decoder->getWidth();
	//h = decoder->getHeight();

	GlutHelper* gluth = new GlutHelper();
	gluth->setFFmpegDecoder(decoder);
	gluth->init(&argc, argv, w, h);

	GlHelper* glh = new GlHelper();
	const char* vs_path = "../Shaders/Shader_2/vertex_shader.glsl";
	const char* fs_path = "../Shaders/Shader_2/fragment_shader.glsl";
	glh->init(w, h, vs_path, fs_path) ;

	gluth->setGlHelper(glh);

	gluth->setCallbackFuncs();
	gluth->loop();


}

