CXX ?= c++
CXXFLAGS ?= -O2
CXXFLAGS += -std=c++17 -Wall -Wextra -Wpedantic

TARGET = MountPointTest

.PHONY: all clean test

all: $(TARGET)

$(TARGET): MountPointTest.cpp ../src/MountPoint.cpp ../src/MountPoint.h
	$(CXX) $(CXXFLAGS) -I../src MountPointTest.cpp ../src/MountPoint.cpp -o $@

test: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
