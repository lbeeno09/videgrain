# CMakeList.txt : CMake project for Videgrain, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(Videgrain)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# collect my source and headers
set(SOURCES
	src/main.cpp
	src/VideoReader.cpp
)

# Find OpenGL Package
find_package(OpenGL REQUIRED)

# Set up GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(deps/glfw)

# Set up glad
add_library(glad STATIC deps/glad/src/glad.c)
target_include_directories(glad PUBLIC deps/glad/include)

# Set up GLM
set(GLM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/glm")
add_subdirectory(${GLM_DIR})

# Set up FFmpeg
set(FFMPEG_DIR "${CMAKE_SOURCE_DIR}/deps/ffmpeg")
include_directories("${FFMPEG_DIR}/include")
link_directories("${FFMPEG_DIR}/lib")

# Set up imgui
set(IMGUI_DIR deps/imgui)
set(IMGUI_SOURCES
	${IMGUI_DIR}/imgui.cpp
	${IMGUI_DIR}/imgui_draw.cpp
	${IMGUI_DIR}/imgui_tables.cpp
	${IMGUI_DIR}/imgui_widgets.cpp
	${IMGUI_DIR}/imgui_demo.cpp
	${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
	${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES})

# Tell compiler where to find headers
target_include_directories(${PROJECT_NAME} PUBLIC
	include
	${IMGUI_DIR}
	${IMGUI_DIR}/backends
	deps/glfw/include
	deps/glad/include
)

# Link everything
target_link_libraries(${PROJECT_NAME}
	avcodec
	avformat
	avutil
	swscale
	OpenGL::GL
	glad
	glfw
	glm::glm
)

# Copy dll to exe dir
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND  ${CMAKE_COMMAND} -E copy_directory
	"${FFMPEG_DIR}/bin"
	"$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)