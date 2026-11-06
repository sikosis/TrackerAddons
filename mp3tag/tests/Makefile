CXX ?= c++
CXXFLAGS ?= -O2
CXXFLAGS += -std=c++17 -Wall -Wextra -Wpedantic

PATTERN_TARGET = PatternMatcherTest
TAG_TARGET = Id3v1TagTest

.PHONY: all clean test

all: $(PATTERN_TARGET) $(TAG_TARGET)

$(PATTERN_TARGET): PatternMatcherTest.cpp ../src/PatternMatcher.cpp \
	../src/PatternMatcher.h ../src/TagData.h
	$(CXX) $(CXXFLAGS) -I../src PatternMatcherTest.cpp \
		../src/PatternMatcher.cpp -o $@

$(TAG_TARGET): Id3v1TagTest.cpp ../src/Id3v1Tag.cpp \
	../src/Id3v1Tag.h ../src/TagData.h
	$(CXX) $(CXXFLAGS) -I../src Id3v1TagTest.cpp ../src/Id3v1Tag.cpp -o $@

test: all
	./$(PATTERN_TARGET)
	temporary_file=$$(mktemp "$${TMPDIR:-/tmp}/mp3tag-test.XXXXXX"); \
		./$(TAG_TARGET) "$$temporary_file"

clean:
	rm -f $(PATTERN_TARGET) $(TAG_TARGET)
