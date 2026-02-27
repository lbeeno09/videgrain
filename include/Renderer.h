#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

/// <summary>
/// Manage GPU texture and uploading pixels
/// </summary>
class Renderer
{
public:
	Renderer();
	~Renderer();

	void init(int width, int height);
	void updateTexture(const std::vector<uint8_t>& pixels, int width, int height);
	
	GLuint getTextureID() const { return textureID; }

private:
	GLuint textureID = 0;
};