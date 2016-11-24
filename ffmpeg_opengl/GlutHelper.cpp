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
	bool res = s_decoder->readFrame();
	if(res) 
	{
		s_glh->drawFrame();
		glutSwapBuffers();
	}
	else if( !res || !s_running)
	{
		glutLeaveMainLoop();
		//exit(0);
	}
}