CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -g
LDFLAGS += -lstdc++fs
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin

# Find all .cpp files in src and its subdirectories, excluding test files
SRCS := $(shell find $(SRC_DIR) -name '*.cpp' -not -name 'test_runner.cpp' -not -name 'test_*.cpp')
# Add the new register system source files explicitly
REGISTER_SRCS := $(SRC_DIR)/vhardware/cpu_registers.cpp
SRCS += $(REGISTER_SRCS)
# Replace src/ with build/ and .cpp with .o for object files
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

TARGET := $(BIN_DIR)/virtcomp

# Assembler test target (minimal dependencies, no CPU execution)
ASSEMBLER_TEST_TARGET := $(BIN_DIR)/test_assembler
ASSEMBLER_TEST_SRCS := $(SRC_DIR)/test/test_assembler.cpp $(filter $(SRC_DIR)/assembler/%.cpp,$(SRCS))
ASSEMBLER_TEST_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(filter $(SRC_DIR)/%.cpp,$(ASSEMBLER_TEST_SRCS))) $(BUILD_DIR)/test/test_assembler.o

# Test framework
TEST_TARGET := $(BIN_DIR)/test_runner
TEST_SRCS := $(filter-out $(SRC_DIR)/main.cpp $(SRC_DIR)/debug/gui.cpp, $(shell find $(SRC_DIR) -name '*.cpp' -not -name 'test_runner.cpp' -not -name 'test_*.cpp'))
TEST_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(TEST_SRCS))
TEST_RUNNER_SRC := $(SRC_DIR)/test/test_runner.cpp
TEST_RUNNER_OBJ := $(BUILD_DIR)/test/test_runner.o

all: $(TARGET)

# Test framework target
$(TEST_TARGET): $(TEST_OBJS) $(TEST_RUNNER_OBJ) $(FMT_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

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

# Fmt library
FMT_DIR := extern/fmt
FMT_SRCS = $(FMT_DIR)/src/format.cc
FMT_OBJS = $(patsubst $(FMT_DIR)/src/%.cc,$(BUILD_DIR)/fmt/%.o,$(FMT_SRCS))

# Add fmt include path to all compile rules
CXXFLAGS += -Iextern/fmt/include

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

# Pattern rule for fmt sources
$(BUILD_DIR)/fmt/%.o: $(FMT_DIR)/src/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS) $(IMGUI_OBJS) $(FMT_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GL_LIBS) $(LDFLAGS)

# Build assembler test
$(ASSEMBLER_TEST_TARGET): $(ASSEMBLER_TEST_OBJS) $(FMT_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Rule for building test_assembler.o in test build directory
$(BUILD_DIR)/test/test_assembler.o: $(SRC_DIR)/test/test_assembler.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

test: $(TARGET)
	./$(TARGET) -t -d

# Run unit tests
unit-test: $(TARGET)
	./$(TARGET) -t

# Build and run unit tests
test-all: unit-test test

# Test assembler
test-assembler: $(ASSEMBLER_TEST_TARGET)
	./$(ASSEMBLER_TEST_TARGET)

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

build: prereqs $(TARGET)
	@echo "Build complete. Run './$(TARGET)' to start the application."
	@echo "Run 'make clean' to remove build artifacts."

.PHONY: clean build prereqs test unit-test test-all test-assembler