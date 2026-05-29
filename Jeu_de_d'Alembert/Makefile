CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -I/usr/local/include
LDFLAGS = -L/usr/local/lib -lraylib -lm -lpthread -ldl -lrt -lX11

SRC = src/main.cpp src/game.cpp src/display.cpp src/score.cpp
OBJ = $(SRC:.cpp=.o)

TARGET = dalembert

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) score.txt

re: clean all

.PHONY: all clean re