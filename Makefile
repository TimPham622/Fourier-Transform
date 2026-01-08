CXX := c++
CXXFLAGS := -std=c++11 -Wall -Wno-missing-braces
CXXFLAGS += $(shell pkg-config --cflags raylib)

LDFLAGS := $(shell pkg-config --libs raylib)

SRC := main.cpp Complex.cpp DFT.cpp FFT.cpp
OBJ := $(SRC:.cpp=.o)
TARGET := fourier

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
