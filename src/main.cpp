// Videgrain.cpp : Defines the entry point for the application.
//

#include "VideoReader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <cstdio>
#include <iostream>
#include <vector>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if(!glfwInit())
        return 1;

    // GL 450
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create window with graphics context
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    GLFWwindow* window = glfwCreateWindow((int)(1280 * main_scale), (int)(800 * main_scale), "VIdegrain", nullptr, nullptr);
    if(window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Init glad
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;

        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;
    // GL 3.3+
    io.ConfigDpiScaleFonts = true;
    io.ConfigDpiScaleViewports = true;

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");

    // Load Fonts
    // - If fonts are not explicitly loaded, Dear ImGui will select an embedded font: either AddFontDefaultVector() or AddFontDefaultBitmap().
    //   This selection is based on (style.FontSizeBase * style.FontScaleMain * style.FontScaleDpi) reaching a small threshold.
    // - You can load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - If a file cannot be loaded, AddFont functions will return a nullptr. Please handle those errors in your code (e.g. use an assertion, display an error and quit).
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use FreeType for higher quality font rendering.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //style.FontSizeBase = 20.0f;
    //io.Fonts->AddFontDefaultVector();
    //io.Fonts->AddFontDefaultBitmap();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);

    // Setup Video Reader
    VideoReader reader;
    GLuint videoTexture;
    std::vector<uint8_t> frameData;
    bool isLoaded = reader.open("test_video.mp4");
    if(isLoaded)
    {
        frameData.resize(reader.width * reader.height * 4);

        // Create Texture for Video
        glGenTextures(1, &videoTexture);
        glBindTexture(GL_TEXTURE_2D, videoTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // prevent stretching on edge

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, reader.width, reader.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }

    double lastTime = glfwGetTime();
    bool isPlaying = true;

    // Main loop
    while(!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if(glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        double currentTime = glfwGetTime();
        double frameDelay = 1.0 / reader.fps;
        if(isPlaying)
        {
            if(currentTime - lastTime >= frameDelay)
            {
                if(isLoaded && reader.readFrame(frameData.data(), reader.width, reader.height))
                {
                    glBindTexture(GL_TEXTURE_2D, videoTexture);
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, reader.width, reader.height, GL_RGBA, GL_UNSIGNED_BYTE, frameData.data());

                    lastTime = currentTime;
                }
                else
                {
                    isPlaying = false;
                }
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();

        // Video Window
        ImGui::Begin("Video");
        {
            if(isLoaded && videoTexture != 0)
            {
                ImVec2 region = ImGui::GetContentRegionAvail();
                float scale = std::min(region.x / reader.width, region.y / reader.height);
                ImVec2 displaySize = ImVec2(reader.width * scale, reader.height * scale);
                
                ImGui::Image((ImTextureID)(uintptr_t)videoTexture, displaySize);
            }
            else
            {
                ImGui::Text("No Video Loaded");
            }
        }
        ImGui::End();

        // Timeline window
        ImGui::Begin("Timeline");
        {
            const char* buttonLabel = isPlaying ? "Pause" : "Play";
            if(ImGui::Button(buttonLabel))
            {
                isPlaying = !isPlaying;
            }
            ImGui::SameLine();

            // Slider
            int currentF = (int)reader.currentFrameIndex;
            if(ImGui::SliderInt("Frame", &currentF, 0, (int)reader.totalFrames))
            {
                //isPlaying = false;
                reader.seekFrame(currentF);
            }

            ImGui::Text("Time: %.2f / %.2f seconds", (double)reader.currentFrameIndex / reader.fps, (double)reader.totalFrames / reader.fps);
        }
        ImGui::End();

        ImGui::Begin("Properties");
        {
            ImGui::Text("File: test_video.mp4");
            ImGui::Separator();
            ImGui::Text("Resolution: %d x %d", reader.width, reader.height);
            ImGui::Text("Frame Rate: %.2f", reader.fps);
            ImGui::Text("Total Frames: %lld", reader.totalFrames);

            if(ImGui::BeginTable("info", 2)) 
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0); ImGui::Text("Current Frame");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%lld", reader.currentFrameIndex);

                ImGui::EndTable();
            }
        }
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    // Cleanup
    reader.close();
    glDeleteTextures(1, &videoTexture);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
