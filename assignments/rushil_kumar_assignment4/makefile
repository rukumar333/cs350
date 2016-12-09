CXX ?= gcc
CXXFLAGS := -g -Wall -Wextra

all: user_level.c user_level_bad.c
	$(CXX) $(CXXFLAGS) user_level.c -o user_level
	$(CXX) $(CXXFLAGS) user_level_bad.c -o user_level_bad

clean:
	rm user_level user_level_bad
