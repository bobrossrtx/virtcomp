CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -g
LDFLAGS += -lstdc++fs
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin

# Find all .cpp files in src and its subdirectories
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
# Replace src/ with build/ and .cpp with .o for object files
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

TARGET := $(BIN_DIR)/virtcomp

all: $(TARGET)

# ImGui/GLFW/OpenGL3 sources
IMGUI_DIR := extern/imgui
IMGUI_BACKEND := $(IMGUI_DIR)/backends
IMGUI_SRCS = \
    $(IMGUI_DIR)/imgui.cpp \
    $(IMGUI_DIR)/imgui_draw.cpp \
    $(IMGUI_DIR)/imgui_tables.cpp \
    $(IMGUI_DIR)/imgui_widgets.cpp \
    $(IMGUI_DIR)/imgui_demo.cpp \
    $(IMGUI_BACKEND)/imgui_impl_glfw.cpp \
    $(IMGUI_BACKEND)/imgui_impl_opengl3.cpp

IMGUI_OBJS = $(patsubst $(IMGUI_DIR)/%.cpp,$(BUILD_DIR)/imgui/%.o,$(filter $(IMGUI_DIR)/%.cpp,$(IMGUI_SRCS))) \
             $(patsubst $(IMGUI_BACKEND)/%.cpp,$(BUILD_DIR)/imgui/backends/%.o,$(filter $(IMGUI_BACKEND)/%.cpp,$(IMGUI_SRCS)))
GL_LIBS = -lglfw -lGLEW -lGLU -lGL -ldl

# Pattern rule to build .o files in build/ mirroring src/ structure
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -Iextern/imgui -Iextern/imgui/backends -c $< -o $@

# Pattern rule for ImGui sources
$(BUILD_DIR)/imgui/%.o: $(IMGUI_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -Iextern/imgui -Iextern/imgui/backends -c $< -o $@

$(BUILD_DIR)/imgui/backends/%.o: $(IMGUI_BACKEND)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -Iextern/imgui -Iextern/imgui/backends -c $< -o $@

$(TARGET): $(OBJS) $(IMGUI_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GL_LIBS) $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

test: $(TARGET)
	./$(TARGET) -t -d

prereqs:
	@echo "Installing system dependencies..."
	sudo apt-get update
	sudo apt-get install -y libglfw3-dev libglew-dev libgl1-mesa-dev xorg-dev
	@if [ ! -d extern/imgui ]; then \
		echo "Cloning Dear ImGui..."; \
		git clone https://github.com/ocornut/imgui.git extern/imgui; \
	else \
		echo "Dear ImGui already present."; \
	fi

.PHONY: all clean test prereqs