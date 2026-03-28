CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -lncurses

BUILD_DIR = build
TARGET = $(BUILD_DIR)/game

SRCS = main.cpp Game.cpp Map.cpp Player.cpp Render.cpp
OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.cpp=.o))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean