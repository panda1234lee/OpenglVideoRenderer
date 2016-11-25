#pragma once
#include "GlHelper.h"
#include "ffmpegDecoder.h"
#include <time.h>
#include <gl/freeglut.h>

#define TIMER 20

class GlutHelper
{
public:
	GlutHelper();
	~GlutHelper();

	inline void init(int *argc, char *argv[], int width, int height)
	{
		glutInit(argc, argv);
		// GLUT_DOUBLE： 双缓冲
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA /*| GLUT_STENCIL | GLUT_DEPTH*/);
		glutInitWindowPosition(100, 100);
		glutInitWindowSize(width, height);
		glutCreateWindow("ImageTransition");

		/*GLenum l = */glewInit();
		if (glewIsSupported("GL_VERSION_2_0"))
			printf("Ready for OpenGL 2.0\n");
		else 
		{
			printf("OpenGL 2.0 not supported\n");
			exit(1);
		}
	}

	inline void setCallbackFuncs()
	{
		glutDisplayFunc(display);
		//glutIdleFunc(&display);
		glutTimerFunc(TIMER, timeFunc, TIMER);

		glutKeyboardFunc(processNormalKeys);
	}

	inline void loop()
	{
		glutMainLoop();
	}

	static void setGlHelper(GlHelper* p)
	{
		s_glh = p;
	}

	static void setFFmpegDecoder(FFmpegDecoder * p)
	{
		s_decoder = p;
	}

	// 定时执行渲染方法
	inline static void timeFunc(int value)
	{
		//clock_t start = clock();
		display();
		//clock_t end = clock();
		//printf("timeFunc = %d ms \n", end - start);

		// 定时循环播放
		//	间隔value ms，并传入value
		glutTimerFunc(value, timeFunc, value);
	}

	inline static void processNormalKeys(unsigned char key, int x, int y) 
	{
		// Esc
		if (key == 27)
		{
			s_running = false;
		}
	}

	inline static void printGLVersionInfo()
	{
		printf("Version: %s\n", glGetString(GL_VERSION));
	}

	// 渲染方法
	static void display();

private:
	static GlHelper *s_glh;
	static FFmpegDecoder * s_decoder;
	static bool s_running;

};

