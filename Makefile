CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -lncurses

TARGET = game

SRCS = main.cpp Game.cpp Map.cpp Player.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean