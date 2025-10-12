#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# You will need GLFW (http://www.glfw.org):
# Linux:
#   apt-get install libglfw-dev
# Mac OS X:
#   brew install glfw
# MSYS2:
#   pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw
#

CXX = g++
#CXX = clang++

# Output
BUILD_DIR := build
BIN_DIR := bin
TARGET = $(BIN_DIR)/chirp

SRC_DIR = src
AUDIO_DIR = src/audio
GUI_DIR = src/gui
IMGUI_DIR = src/external/imgui

# Source files
SRCS := $(IMGUI_DIR)/imgui.cpp \
		$(IMGUI_DIR)/imgui_demo.cpp \
		$(IMGUI_DIR)/imgui_draw.cpp \
		$(IMGUI_DIR)/imgui_tables.cpp \
		$(IMGUI_DIR)/imgui_widgets.cpp \
		$(IMGUI_DIR)/backends/imgui_impl_glfw.cpp \
		$(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp \
		$(AUDIO_DIR)/AudioEngine.cpp \
		$(AUDIO_DIR)/AudioBackend.cpp \
		$(AUDIO_DIR)/AudioProcessor.cpp \
		$(AUDIO_DIR)/Frequency.cpp \
		$(GUI_DIR)/GUIManager.cpp \
		src/main.cpp

# Object files in build/, preserving directory structure
OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

UNAME_S := $(shell uname -s)
LINUX_GL_LIBS = -lGL

CXXFLAGS = -std=c++23 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(SRC_DIR) -I$(AUDIO_DIR) -I$(GUI_DIR)
CXXFLAGS += -g -Wall -Wformat
LIBS = -lportaudio

##---------------------------------------------------------------------
## OPENGL ES
##---------------------------------------------------------------------

## This assumes a GL ES library available in the system, e.g. libGLESv2.so
# CXXFLAGS += -DIMGUI_IMPL_OPENGL_ES2
# LINUX_GL_LIBS = -lGLESv2

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += $(LINUX_GL_LIBS) `pkg-config --static --libs glfw3`

	CXXFLAGS += `pkg-config --cflags glfw3`
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	LIBS += -L/usr/local/lib -L/opt/local/lib -L/opt/homebrew/lib
	#LIBS += -lglfw3
	LIBS += -lglfw

	CXXFLAGS += -I/usr/local/include -I/opt/local/include -I/opt/homebrew/include
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(OS), Windows_NT)
	ECHO_MESSAGE = "MinGW"
	LIBS += -lglfw3 -lgdi32 -lopengl32 -limm32

	CXXFLAGS += `pkg-config --cflags glfw3`
	CFLAGS = $(CXXFLAGS)
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

# Default target
all: $(TARGET)
	@echo "Build complete for $(ECHO_MESSAGE)"

debug: CXXFLAGS += -g -O0
debug: $(TARGET)
	@echo "Build complete for $(ECHO_MESSAGE)"

# Link the main binary
$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

# Compile .cpp files into build/ preserving folder structure
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean