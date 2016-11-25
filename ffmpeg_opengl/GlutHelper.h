#pragma once
#include "GlHelper.h"
#include "ffmpegDecoder.h"
#include <time.h>
#include <gl/freeglut.h>

#define TIMER 20

class GlutHelper
{
public:
	static GlutHelper* getInstance()
	{
		if (!s_instance)
		{
			s_instance = new GlutHelper();
		}

		return s_instance;
	}

	static void release()
	{
		if(s_instance)
			delete s_instance;
		s_instance = NULL;
	}

	inline void init(int *argc, char *argv[], int width, int height)
	{
		glutInit(argc, argv);
		// GLUT_DOUBLE�� ˫����
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

	// ��ʱִ����Ⱦ����
	inline static void timeFunc(int value)
	{
		//clock_t start = clock();
		display();
		//clock_t end = clock();
		//printf("timeFunc = %d ms \n", end - start);

		// ��ʱѭ������
		//	���value ms��������value
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

	// ��Ⱦ����
	static void display();
private:
	GlutHelper();
	~GlutHelper();

private:
	static GlutHelper *s_instance;
	static GlHelper *s_glh;
	static FFmpegDecoder * s_decoder;
	static bool s_running;

};

