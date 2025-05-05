CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -g

TARGET = computer_club


SOURCES = main.cpp library.hpp
HEADERS = library.hpp

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
