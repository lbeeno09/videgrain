#pragma once

#include "VideoReader.h"
#include "AudioReader.h"
#include "Renderer.h"
#include "UI.h"

#include <miniaudio.h>

#include <cstdint>
#include <string>
#include <vector>

class VideoPlayer
{
public:
	bool load(const char* path);
	void update(double dt);
	void render();
	static void audioCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

private:

	VideoReader videoReader;
	AudioReader audioReader;
	Renderer renderer;
	UI ui;
	ma_device device;

	std::vector<uint8_t> pixelBuffer;

	float seekSeconds = 0.0f;
	double currentTime = 0.0;

	bool isPaused = false;
	bool needsSeek = false;
};