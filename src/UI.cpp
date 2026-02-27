#include "UI.h"

#include <cstdio>

void UI::render(GLuint textureID, int width, int height, float& seekSeconds, float totalDuration, bool& isPaused, bool& needsSeek)
{
	{
		ImGui::Begin("Viewport");
		ImVec2 windowSize = ImGui::GetContentRegionAvail();
		float videoAspect = (float)width / (float)height;
		float windowAspect = windowSize.x / windowSize.y;

		ImVec2 displaySize;
		if(videoAspect > windowAspect)
		{
			displaySize.x = windowSize.x;
			displaySize.y = windowSize.x / videoAspect;
		}
		else
		{
			displaySize.x = windowSize.y * videoAspect;
			displaySize.y = windowSize.y;
		}

		ImGui::SetCursorPos(ImVec2((windowSize.x - displaySize.x) * 0.5f, (windowSize.y - displaySize.y) * 0.5f + ImGui::GetCursorPosY()));

		ImGui::Image((void*)(intptr_t)textureID, displaySize);
		ImGui::End();
	}

	{
		ImGui::Begin("Timeline"); // Controls

		float windowWidth = ImGui::GetWindowWidth();
		float windowHeight = ImGui::GetWindowHeight();

		// Center Buttons
		float side = 40.0f;
		float spacing = ImGui::GetStyle().ItemSpacing.x;
		float groupWidth = (side * 3) + (spacing * 2);

		ImGui::SetCursorPosX((windowWidth - groupWidth) * 0.5f);
		ImGui::SetCursorPosY(windowHeight * 0.2f);
		if(ImGui::Button("<<", ImVec2(side, side)))
		{
			seekSeconds -= 5.0f;
			if(seekSeconds < 0.0f)
			{
				seekSeconds = 0.0f;
			}
			needsSeek = true;
		}
		ImGui::SameLine();
		if(ImGui::Button(isPaused ? ">" : "||", ImVec2(side, side)))
		{
			isPaused = !isPaused;
		}
		ImGui::SameLine();
		if(ImGui::Button(">>", ImVec2(side, side)))
		{
			seekSeconds += 5.0f;
			if(seekSeconds > totalDuration)
			{
				seekSeconds = totalDuration;
			}
			needsSeek = true;
		}


		// Play Time
		float barHeight = 30.0f;
		ImGui::SetCursorPosY(windowHeight - barHeight - 10.0f);

		float timerWidth = 110.0f;
		float slider_width = windowWidth - timerWidth - spacing;

		ImGui::PushItemWidth(slider_width);
		if(ImGui::SliderFloat("##timeline", &seekSeconds, 0.0f, totalDuration, ""))
		{
			needsSeek = true;
		}
		ImGui::PopItemWidth();

		ImGui::SameLine();
		int curM = (int)seekSeconds / 60;
		int curS = (int)seekSeconds % 60;
		int totM = (int)totalDuration / 60;
		int totS = (int)totalDuration % 60;

		ImGui::Text("%02d:%02d / %02d:%02d", curM, curS, totM, totS);


		ImGui::End();
	}
}
