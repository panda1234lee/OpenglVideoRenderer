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
	FFmpegDecoder();
	~FFmpegDecoder();

	int init(const char* file_name, int dst_w, int dst_h);
	int findStream();
	int findAndOpenCodec();
	void makeFrame(AVFrame** frame, int width, int height);
	void clear();
	bool readFrame();

	void close()
	{
		avformat_close_input(&m_fmt_ctx);
	}

	int getWidth()
	{
		return m_codec_ctx->width;
	}

	int getHeight()
	{
		return m_codec_ctx->height;
	}

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
};

