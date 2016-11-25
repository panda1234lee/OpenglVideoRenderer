#include "stdafx.h"
#include "GlutHelper.h"

GlutHelper* GlutHelper::s_instance = NULL;
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
	bool res = s_decoder->readAVFrame();
	//clock_t end = clock();
	//printf("readFrame = %d ms \n", end - start);

	if(res) 
	{
		//clock_t start = clock();
		s_glh->deliverDataToTexture(
			s_decoder->getDstWidth(), 
			s_decoder->getDstHeight(), 
			s_decoder->getGLFrameData());

		s_glh->drawFrame();
		//clock_t end = clock();
		//printf("drawframe = %d ms \n", end - start);

		glutSwapBuffers();
	}

	if( !res || !s_running)
	{
		s_glh->release();

		// close
		s_decoder->close();
		s_decoder->release();

		glutLeaveMainLoop();
		//exit(0);
	}
}