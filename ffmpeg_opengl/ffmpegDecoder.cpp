#include "stdafx.h"
#include "FFmpegDecoder.h"
#include "GlHelper.h"

#include <iostream>

#define VIDEO_INFO 1

//const char* file_name = "../video/ink.mov";

FFmpegDecoder::FFmpegDecoder()
{
    m_fmt_ctx = NULL;
    m_stream_idx = -1;
    m_video_stream = NULL;
    m_codec_ctx = NULL;
    m_decoder = NULL;
    m_gl_frame = NULL;
    m_conv_ctx = NULL;
}


FFmpegDecoder::~FFmpegDecoder()
{
    clear();
}

int FFmpegDecoder::init(const char *file_name)
{
    // initialize libav
    av_register_all();
    //avformat_network_init();

    // open video
    if (avformat_open_input(&m_fmt_ctx, file_name, NULL, NULL) < 0)
    {
        std::cout << "failed to open input" << std::endl;
        clear();
        return -1;
    }

#ifdef VIDEO_INFO
	// dump debug info
	av_dump_format(m_fmt_ctx, 0, file_name, 0);
#endif

	findStream();

	findAndOpenCodec();

    makeFrame(&m_gl_frame, m_codec_ctx->width, m_codec_ctx->height);

    m_packet = (AVPacket *)av_malloc(sizeof(AVPacket));
}

int FFmpegDecoder::findStream()
{
	// find stream info
	if (avformat_find_stream_info(m_fmt_ctx, NULL) < 0)
	{
		std::cout << "failed to get stream info" << std::endl;
		clear();
		return -1;
	}

	// find the video stream
	for (unsigned int i = 0; i < m_fmt_ctx->nb_streams; ++i)
	{
		if (m_fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			m_stream_idx = i;
			break;
		}
	}

	if (m_stream_idx == -1)
	{
		std::cout << "failed to find video stream" << std::endl;
		clear();
		return -1;
	}

	m_video_stream = m_fmt_ctx->streams[m_stream_idx];

	return 0;
}

int FFmpegDecoder::findAndOpenCodec()
{
	m_codec_ctx = m_video_stream->codec;

	// find the m_decoder
	m_decoder = avcodec_find_decoder(m_codec_ctx->codec_id);
	if (m_decoder == NULL)
	{
		std::cout << "failed to find m_decoder" << std::endl;
		clear();
		return -1;
	}

	// open the m_decoder
	if (avcodec_open2(m_codec_ctx, m_decoder, NULL) < 0)
	{
		std::cout << "failed to open codec" << std::endl;
		clear();
		return -1;
	}

	return 0;
}

void FFmpegDecoder::makeFrame(AVFrame** frame, int width, int height)
{
	if (*frame)
	{
		av_free(*frame);
		*frame = NULL;
	}

    // allocate the video frames
    *frame = av_frame_alloc();

    int size = avpicture_get_size(AV_PIX_FMT_RGB24, width,
                                  height);
    uint8_t *internal_buffer = (uint8_t *)av_malloc(size * sizeof(uint8_t));

    avpicture_fill((AVPicture *)*frame, internal_buffer, AV_PIX_FMT_RGB24,
                   width, height);
}


void FFmpegDecoder::clear()
{
    if (m_gl_frame)
        av_free(m_gl_frame);
    if (m_packet)
        av_free(m_packet);
    if (m_codec_ctx)
        avcodec_close(m_codec_ctx);
    if (m_fmt_ctx)
        avformat_free_context(m_fmt_ctx);

    m_fmt_ctx = NULL;
    m_stream_idx = -1;
    m_video_stream = NULL;
    m_codec_ctx = NULL;
    m_decoder = NULL;
    m_gl_frame = NULL;
    m_conv_ctx = NULL;
}

// read a video frame
bool FFmpegDecoder::readFrame()
{
    if (av_read_frame(m_fmt_ctx, m_packet) < 0)
    {
        av_free_packet(m_packet);
        return false;
    }

    // 如果是视频包
    if (m_packet->stream_index == m_stream_idx)
    {
        int frame_finished = 0;

        AVFrame *av_frame = av_frame_alloc();

        if (avcodec_decode_video2(m_codec_ctx, av_frame, &frame_finished,
                                  m_packet) < 0)
        {
            av_free_packet(m_packet);
            return false;
        }

        // 解码成功
        if (frame_finished)
        {
            if (!m_conv_ctx)
            {
                m_conv_ctx = sws_getContext(m_codec_ctx->width,
                                            m_codec_ctx->height, m_codec_ctx->pix_fmt,
                                            m_codec_ctx->width, m_codec_ctx->height, AV_PIX_FMT_RGB24,
                                            SWS_BICUBIC, NULL, NULL, NULL);
            }

            sws_scale(m_conv_ctx, av_frame->data, av_frame->linesize, 0,
                      m_codec_ctx->height, m_gl_frame->data, m_gl_frame->linesize);

            GlHelper::deliverDataToTexture(m_codec_ctx->width, m_codec_ctx->height, m_gl_frame->data[0]);
        }

        if (av_frame)
            av_free(av_frame);
        av_frame = NULL;
    }

    av_free_packet(m_packet);

    return true;
}
