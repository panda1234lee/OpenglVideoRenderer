#include "stdafx.h"
#include "GlutHelper.h"


GlHelper* GlutHelper::s_glh = NULL;
FFmpegDecoder* GlutHelper::s_decoder = NULL;
bool GlutHelper::s_running = true;

GlutHelper::GlutHelper()
{
}

GlutHelper::~GlutHelper()
{
	if (s_glh)
	{
		s_glh = NULL;
	}	
}

void GlutHelper::display()
{
	if (NULL == s_glh)
	{
		printf("GlHelper Pointer has not been set yet! \n");
		exit(1);
	}

	if (NULL == s_decoder)
	{
		printf("FFmpegDecoder Pointer has not been set yet! \n");
		exit(1);
	}

	// run the application mainloop
	//clock_t start = clock();
	bool res = s_decoder->readFrame();
	//clock_t end = clock();
	//printf("readFrame = %d ms \n", end - start);

	if(res) 
	{
		//clock_t start = clock();
		s_glh->drawFrame();
		//clock_t end = clock();
		//printf("drawframe = %d ms \n", end - start);

		glutSwapBuffers();
	}

	if( !res || !s_running)
	{
		delete s_glh;
		s_glh = NULL;

		// close
		s_decoder->close();
		delete s_decoder;
		s_decoder = NULL;

		glutLeaveMainLoop();
		//exit(0);
	}
}