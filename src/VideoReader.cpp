#include "VideoReader.h"
#include <iostream>

VideoReader::VideoReader() : formatCtx(nullptr), codecCtx(nullptr), swsCtx(nullptr), frame(nullptr), packet(nullptr), videoStreamIdx(-1), width(0), height(0), fps(0.0), totalFrames(0), currentFrameIndex(0)
{
}

VideoReader::~VideoReader()
{
	close();
}

bool VideoReader::open(const char* filename)
{
	if(avformat_open_input(&formatCtx, filename, nullptr, nullptr) < 0)
	{
		return false;
	}
	if(avformat_find_stream_info(formatCtx, nullptr) < 0)
	{
		return false;
	}

	for(int i = 0; i < formatCtx->nb_streams; i++)
	{
		if(formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoStreamIdx = i;
			break;
		}
	}
	if(videoStreamIdx == -1)
	{
		return false;
	}

	const AVCodec* codec = avcodec_find_decoder(formatCtx->streams[videoStreamIdx]->codecpar->codec_id);
	codecCtx = avcodec_alloc_context3(codec);
	avcodec_parameters_to_context(codecCtx, formatCtx->streams[videoStreamIdx]->codecpar);
	if(avcodec_open2(codecCtx, codec, nullptr) < 0)
	{
		return false;
	}

	fps = av_q2d(formatCtx->streams[videoStreamIdx]->avg_frame_rate);
	int64_t frames = formatCtx->streams[videoStreamIdx]->nb_frames;
	if(frames <= 0)
	{
		double durationSec = (double)formatCtx->duration / AV_TIME_BASE;

		frames = (int64_t)(durationSec * fps);
	}
	totalFrames = frames;

	width = codecCtx->width;
	height = codecCtx->height;
	frame = av_frame_alloc();
	packet = av_packet_alloc();

	return true;
}

bool VideoReader::readFrame(uint8_t* outData, int targetWidth, int targetHeight)
{
	while(av_read_frame(formatCtx, packet) >= 0)
	{
		if(packet->stream_index == videoStreamIdx)
		{
			avcodec_send_packet(codecCtx, packet);
			int response = avcodec_receive_frame(codecCtx, frame);
			if(response == AVERROR(EAGAIN) || response == AVERROR_EOF)
			{
				// wait for more packet
				av_packet_unref(packet);
				continue;
			}
			else if(response < 0)
			{
				return false;
			}

			swsCtx = sws_getCachedContext(swsCtx, frame->width, frame->height, (AVPixelFormat)frame->format, targetWidth, targetHeight, AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr);
			if(!swsCtx)
			{
				return false;
			}
			
			uint8_t* dest[4] = { outData, nullptr, nullptr, nullptr };
			int destLinesize[4] = { targetWidth * 4, 0, 0, 0 };
			sws_scale(swsCtx, frame->data, frame->linesize, 0, frame->height, dest, destLinesize);

			av_packet_unref(packet);
			
			currentFrameIndex++;
			return true;
		}
		av_packet_unref(packet);
	}
	return false;
}

bool VideoReader::seekFrame(int64_t frameIdx)
{
	if(!formatCtx || videoStreamIdx == -1)
	{
		return false;
	}

	// convert frame index to FFmpeg timestamp
	int64_t targetTS = av_rescale_q(frameIdx, AVRational{ 1, (int)fps }, formatCtx->streams[videoStreamIdx]->time_base);

	// Seek nearest keyframe BEFORE target
	if(av_seek_frame(formatCtx, videoStreamIdx, targetTS, AVSEEK_FLAG_BACKWARD) < 0)
	{
		return false;
	}

	currentFrameIndex = frameIdx;

	return true;
}

void VideoReader::close()
{
	if(swsCtx)
	{
		sws_freeContext(swsCtx);
		swsCtx = nullptr;
	}
	if(codecCtx)
	{
		avcodec_free_context(&codecCtx);
		codecCtx = nullptr;
	}
	if(formatCtx)
	{
		avformat_close_input(&formatCtx);
		formatCtx = nullptr;
	}
	if(frame)
	{
		av_frame_free(&frame);
		frame = nullptr;
	}
	if(packet)
	{
		av_packet_free(&packet);
		packet = nullptr;
	}
}
