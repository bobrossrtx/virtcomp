Makefile
# Compiler and flags
CXX        := g++
CXXFLAGS   := -Wall -Wextra -std=c++17 -g -MMD -MP -Iextern/fmt/include
# LDFLAGS  += -lstdc++fs

# Directories
SRC_DIR    := src
BUILD_DIR  := build
BIN_DIR    := bin

# Source and object files
SRCS       := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS       := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
TARGET     := $(BIN_DIR)/virtcomp

# ImGui/GLFW/OpenGL3 sources
IMGUI_DIR      := extern/imgui
IMGUI_BACKEND  := $(IMGUI_DIR)/backends
IMGUI_SRCS     := \
	$(IMGUI_DIR)/imgui.cpp \
	$(IMGUI_DIR)/imgui_draw.cpp \
	$(IMGUI_DIR)/imgui_tables.cpp \
	$(IMGUI_DIR)/imgui_widgets.cpp \
	$(IMGUI_DIR)/imgui_demo.cpp \
	$(IMGUI_BACKEND)/imgui_impl_glfw.cpp \
	$(IMGUI_BACKEND)/imgui_impl_opengl3.cpp

IMGUI_OBJS := \
	$(patsubst $(IMGUI_DIR)/%.cpp,$(BUILD_DIR)/imgui/%.o,$(filter $(IMGUI_DIR)/%.cpp,$(IMGUI_SRCS))) \
	$(patsubst $(IMGUI_BACKEND)/%.cpp,$(BUILD_DIR)/imgui/backends/%.o,$(filter $(IMGUI_BACKEND)/%.cpp,$(IMGUI_SRCS)))

GL_LIBS := -lglfw -lGLEW -lGLU -lGL -ldl

# Fmt library
FMT_DIR   := extern/fmt
FMT_SRCS  := $(FMT_DIR)/src/format.cc
FMT_OBJS  := $(patsubst $(FMT_DIR)/src/%.cc,$(BUILD_DIR)/fmt/%.o,$(FMT_SRCS))

# Default target
all: $(TARGET)

# Helper: create directory for any target
%.dir:
	@mkdir -p "$*"

# Pattern rules for building object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(dir $@).dir
	$(CXX) $(CXXFLAGS) -Iextern/imgui -Iextern/imgui/backends -c $< -o $@

$(BUILD_DIR)/imgui/%.o: $(IMGUI_DIR)/%.cpp | $(dir $@).dir
	$(CXX) $(CXXFLAGS) -Iextern/imgui -Iextern/imgui/backends -c $< -o $@

$(BUILD_DIR)/imgui/backends/%.o: $(IMGUI_BACKEND)/%.cpp | $(dir $@).dir
	$(CXX) $(CXXFLAGS) -Iextern/imgui -Iextern/imgui/backends -c $< -o $@

$(BUILD_DIR)/fmt/%.o: $(FMT_DIR)/src/%.cc | $(dir $@).dir
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link target
$(TARGET): $(OBJS) $(IMGUI_OBJS) $(FMT_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GL_LIBS) $(LDFLAGS)

# Clean build artifacts
clean:
	$(RM) -rf $(BUILD_DIR) $(BIN_DIR)

# Run tests
test: $(TARGET)
	./$(TARGET) -t -d

# Install prerequisites and clone externals if needed
prereqs:
	@if ! dpkg -s libglfw3-dev libglew-dev libgl1-mesa-dev xorg-dev >/dev/null 2>&1; then \
		echo "Installing required system libraries..."; \
		echo "This may take a while..."; \
		sudo apt-get -qq update; \
		sudo apt-get -qq install -y libglfw3-dev libglew-dev libgl1-mesa-dev xorg-dev; \
	else \
		echo "All required system libraries are already installed."; \
	fi
	@if [ ! -d extern/imgui ]; then \
		echo "Cloning Dear ImGui..."; \
		git clone https://github.com/ocornut/imgui extern/imgui; \
		cd extern/imgui && git checkout docking; \
	else \
		echo "Dear ImGui already present."; \
	fi
	@if [ ! -d extern/fmt ]; then \
		echo "Cloning fmt..."; \
		git clone https://github.com/fmtlib/fmt.git extern/fmt; \
	else \
		echo "fmt already present."; \
	fi

# Build with prerequisites
build: prereqs $(TARGET)
	@echo "Build complete. Run './$(TARGET)' to start the application."
	@echo "Run 'make clean' to remove build artifacts."

# Include dependency files if they exist
-include $(OBJS:.o=.d) $(IMGUI_OBJS:.o=.d) $(FMT_OBJS:.o=.d)

.PHONY: all clean build prereqs test