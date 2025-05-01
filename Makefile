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

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Pattern rule to build .o files in build/ mirroring src/ structure
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

test: $(TARGET)
	./$(TARGET) -t -d

.PHONY: all clean