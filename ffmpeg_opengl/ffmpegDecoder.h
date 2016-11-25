#pragma once

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavdevice/avdevice.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#include <time.h>
}

class FFmpegDecoder
{
public:
	static FFmpegDecoder* getInstance()
	{
		if (!s_instance)
		{
			s_instance = new FFmpegDecoder();
		}

		return s_instance;
	}

	static void release()
	{
		if(s_instance)
			delete s_instance;
		s_instance = NULL;
	}

	int init(const char* file_name, int dst_w, int dst_h);
	int findStream();
	int findAndOpenCodec();
	void makeAVFrame(AVFrame** frame, const int width, const int height);
	void clear();
	bool readAVFrame();

	void close()
	{
		avformat_close_input(&m_fmt_ctx);
	}

	unsigned char* getGLFrameData()
	{
		return m_gl_frame->data[0];
	}

	int getSrcWidth()
	{
		return m_codec_ctx->width;
	}

	int getSrcHeight()
	{
		return m_codec_ctx->height;
	}

	int getDstWidth()
	{
		return m_dst_w;
	}

	int getDstHeight()
	{
		return m_dst_h;
	}

private:
	FFmpegDecoder();
	~FFmpegDecoder();

private:
	int m_dst_w, m_dst_h;
	int m_stream_idx;

	AVFormatContext *m_fmt_ctx;
	AVStream *m_video_stream;
	AVCodecContext *m_codec_ctx;
	AVCodec *m_decoder;
	AVPacket *m_packet;
	AVFrame *m_gl_frame;
	struct SwsContext *m_conv_ctx;

	static FFmpegDecoder* s_instance;
};

