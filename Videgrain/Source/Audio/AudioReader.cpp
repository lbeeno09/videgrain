//#include "AudioReader.h"
//
//AudioReader::AudioReader() : codecCtx(nullptr), swrCtx(nullptr), streamIdx(-1)
//{
//	frame = av_frame_alloc();
//}
//
//AudioReader::~AudioReader()
//{
//	if(swrCtx)
//	{
//		swr_free(&swrCtx);
//		swrCtx = nullptr;
//	}
//	if(codecCtx)
//	{
//		avcodec_free_context(&codecCtx);
//		codecCtx = nullptr;
//	}
//
//	av_frame_free(&frame);
//}
//
//bool AudioReader::open(AVFormatContext* formatCtx)
//{
//	for(unsigned int i = 0; i < formatCtx->nb_streams; i++)
//	{
//		if(formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
//		{
//			streamIdx = i;
//			break;
//		}
//	}
//	if(streamIdx == -1)
//	{
//		return false;
//	}
//
//	AVCodecParameters* params = formatCtx->streams[streamIdx]->codecpar;
//	const AVCodec* codec = avcodec_find_decoder(params->codec_id);
//	codecCtx = avcodec_alloc_context3(codec);
//	avcodec_parameters_to_context(codecCtx, params);
//	avcodec_open2(codecCtx, codec, nullptr);
//
//	AVChannelLayout outLayout;
//	av_channel_layout_default(&outLayout, 2);
//
//	swr_alloc_set_opts2(&swrCtx, &outLayout, AV_SAMPLE_FMT_FLT, 48000, &codecCtx->ch_layout, codecCtx->sample_fmt, codecCtx->sample_rate, 0, nullptr);
//	swr_init(swrCtx);
//
//	return true;
//}
//
//void AudioReader::decodePacket(AVPacket* pkt)
//{
//	if(avcodec_send_packet(codecCtx, pkt) < 0)
//	{
//		return;
//	}
//
//	while(avcodec_receive_frame(codecCtx, frame) == 0)
//	{
//		size_t oldSize = pcmBuffer.size();
//		pcmBuffer.resize(oldSize + frame->nb_samples * 2);
//
//		uint8_t* outPtr = (uint8_t*)(&pcmBuffer[oldSize]);
//		swr_convert(swrCtx, &outPtr, frame->nb_samples, (const uint8_t**)frame->data, frame->nb_samples);
//	}
//}
//
//void AudioReader::flush()
//{
//	if(swrCtx)
//	{
//		swr_init(swrCtx);
//	}
//	if(codecCtx)
//	{
//		avcodec_flush_buffers(codecCtx);
//	}
//}
