//#include "VideoReader.h"
//#include <iostream>
//
//VideoReader::VideoReader() : width(0), height(0), fps(0.0), totalFrames(0), currentFrameIndex(0), formatCtx(nullptr), codecCtx(nullptr), frame(nullptr), swsCtx(nullptr), videoStreamIdx(-1)
//{
//	frame = av_frame_alloc();
//}
//
//VideoReader::~VideoReader()
//{
//	close();
//
//	av_frame_free(&frame);
//}
//
//bool VideoReader::open(const char* filename)
//{
//	if(avformat_open_input(&formatCtx, filename, nullptr, nullptr) < 0)
//	{
//		return false;
//	}
//	if(avformat_find_stream_info(formatCtx, nullptr) < 0)
//	{
//		return false;
//	}
//
//	for(unsigned int i = 0; i < formatCtx->nb_streams; i++)
//	{
//		if(formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
//		{
//			videoStreamIdx = i;
//			break;
//		}
//	}
//	if(videoStreamIdx == -1)
//	{
//		return false;
//	}
//
//	AVCodecParameters* params = formatCtx->streams[videoStreamIdx]->codecpar;
//	const AVCodec* codec = avcodec_find_decoder(params->codec_id);
//	codecCtx = avcodec_alloc_context3(codec);
//	avcodec_parameters_to_context(codecCtx, params);
//	if(avcodec_open2(codecCtx, codec, nullptr) < 0)
//	{
//		return false;
//	}
//
//	width = codecCtx->width;
//	height = codecCtx->height;
//	fps = av_q2d(formatCtx->streams[videoStreamIdx]->avg_frame_rate);
//	totalFrames = formatCtx->streams[videoStreamIdx]->nb_frames;
//
//	return true;
//}
//
//void VideoReader::decodePacket(AVPacket* pkt, std::vector<uint8_t>& buffer)
//{
//	if(avcodec_send_packet(codecCtx, pkt) < 0)
//	{
//		return;
//	}
//
//	while(avcodec_receive_frame(codecCtx, frame) == 0)
//	{
//		swsCtx = sws_getCachedContext(swsCtx, width, height, codecCtx->pix_fmt, width, height, AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr);
//
//		uint8_t* dest[4] = { buffer.data(), nullptr, nullptr, nullptr};
//		int destLineSize[4] = { width * 4, 0, 0, 0 };
//		sws_scale(swsCtx, frame->data, frame->linesize, 0, height, dest, destLineSize);
//
//		currentFrameIndex++;
//	}
//}
//
//void VideoReader::seekFrame(int64_t frameIdx)
//{
//	av_seek_frame(formatCtx, videoStreamIdx, frameIdx, AVSEEK_FLAG_BACKWARD);
//	avcodec_flush_buffers(codecCtx);
//	currentFrameIndex = frameIdx;
//}
//
//void VideoReader::close()
//{
//	if(swsCtx)
//	{
//		sws_freeContext(swsCtx);
//		swsCtx = nullptr;
//	}
//	if(codecCtx)
//	{
//		avcodec_free_context(&codecCtx);
//		codecCtx = nullptr;
//	}
//	if(formatCtx)
//	{
//		avformat_close_input(&formatCtx);
//		formatCtx = nullptr;
//	}
//}
