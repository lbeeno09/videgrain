//#include "VideoPlayer.h"
//
//bool VideoPlayer::load(const char* path)
//{
//	if(!videoReader.open(path))
//	{
//		return false;
//	}
//
//	audioReader.open(videoReader.GetFormatCtx());
//	renderer.init(videoReader.width, videoReader.height);
//	pixelBuffer.resize(videoReader.width * videoReader.height * 4);
//	ma_device_config config = ma_device_config_init(ma_device_type_playback);
//	config.playback.format = ma_format_f32;
//	config.playback.channels = 2;
//	config.sampleRate = 48000;
//	config.dataCallback = audioCallback;
//	config.pUserData = this;
//
//	return ma_device_init(nullptr, &config, &device) == MA_SUCCESS && ma_device_start(&device) == MA_SUCCESS;
//}
//
//void VideoPlayer::update(double dt)
//{
//	if(isPaused)
//	{
//		return;
//	}
//
//	if(needsSeek)
//	{
//		audioReader.flush();
//		videoReader.seekFrame((int64_t)(seekSeconds * videoReader.fps));
//		needsSeek = false;
//
//		return;
//	}
//
//	AVPacket* pkt = av_packet_alloc();
//	while(audioReader.pcmBuffer.size() < 19200)
//	{
//		if(av_read_frame(videoReader.GetFormatCtx(), pkt) < 0)
//		{
//			break;
//		}
//
//		if(pkt->stream_index == videoReader.GetVideoStreamIdx())
//		{
//			videoReader.decodePacket(pkt, pixelBuffer);
//		}
//		else if(pkt->stream_index == audioReader.streamIdx)
//		{
//			audioReader.decodePacket(pkt);
//		}
//		av_packet_unref(pkt);
//	}
//	av_packet_free(&pkt);
//
//	renderer.updateTexture(pixelBuffer, videoReader.width, videoReader.height);
//	currentTime = (double)videoReader.currentFrameIndex / videoReader.fps;;
//	seekSeconds = (float)currentTime;
//}
//
//void VideoPlayer::render()
//{
//	ui.render(renderer.getTextureID(), videoReader.width, videoReader.height, seekSeconds, (float)videoReader.totalFrames / videoReader.fps, isPaused, needsSeek);
//}
//
//void VideoPlayer::audioCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
//{
//	VideoPlayer* player = (VideoPlayer*)pDevice->pUserData;
//	if(!player || player->isPaused)
//	{
//		return;
//	}
//
//	size_t requested = frameCount * 2;
//	if(player->audioReader.pcmBuffer.size() >= requested)
//	{
//		memcpy(pOutput, player->audioReader.pcmBuffer.data(), requested * sizeof(float));
//		player->audioReader.pcmBuffer.erase(player->audioReader.pcmBuffer.begin(), player->audioReader.pcmBuffer.begin() + requested);
//	}
//	else
//	{
//		memset(pOutput, 0, requested * sizeof(float));
//	}
//}
