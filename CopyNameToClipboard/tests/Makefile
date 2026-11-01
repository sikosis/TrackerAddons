CXX ?= c++
CXXFLAGS ?= -O2
CXXFLAGS += -std=c++17 -Wall -Wextra -Wpedantic

TARGET = ClipboardTextTest
SOURCES = ClipboardTextTest.cpp ../src/ClipboardText.cpp

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(SOURCES) ../src/ClipboardText.h
	$(CXX) $(CXXFLAGS) -I../src $(SOURCES) -o $@

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
