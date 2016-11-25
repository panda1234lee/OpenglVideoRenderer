// ffmpeg_opengl.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "GlutHelper.h"
#include "GlHelper.h"
#include "FFmpegDecoder.h"

#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char *argv[])
{
	int w = 643, h = 480;

	FFmpegDecoder * decoder = FFmpegDecoder::getInstance();
	decoder->init("../video/ink.mov", w, h);	// shining_particle.mov, ink.mov  [有的mp4格式会莫名卡顿]
	
	w = decoder->getDstWidth();
	h = decoder->getDstHeight();

	GlutHelper* gluth = GlutHelper::getInstance();
	gluth->setFFmpegDecoder(decoder);
	gluth->init(&argc, argv, w, h);

	GlHelper* glh = GlHelper::getInstance();
	const char* vs_path = "../Shaders/Shader_2/vertex_shader.glsl";	// Shader_3
	const char* fs_path = "../Shaders/Shader_2/fragment_shader.glsl";
	glh->init(w, h, vs_path, fs_path) ;

	gluth->setGlHelper(glh);

	gluth->setCallbackFuncs();
	gluth->loop();


}

