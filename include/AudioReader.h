#pragma once

#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/channel_layout.h>
}

class AudioReader
{
public:
	AudioReader();
	~AudioReader();

	bool open(AVFormatContext* formatCtx);
	void decodePacket(AVPacket* pkt);
	void flush();

	std::vector<float> pcmBuffer;
	int streamIdx;

private:
	AVCodecContext* codecCtx;
	SwrContext* swrCtx;
	AVFrame* frame;
};