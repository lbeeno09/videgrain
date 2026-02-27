#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"

class UI
{
public:
	void render(GLuint textureID, int width, int height, float& seekSeconds, float totalDuration, bool& isPaused, bool& needsSeek);
};