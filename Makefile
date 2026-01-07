# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wno-missing-braces

LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Project files
SRC = main.cpp Complex.cpp DFT.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = fourier_draw

# Build rules
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)