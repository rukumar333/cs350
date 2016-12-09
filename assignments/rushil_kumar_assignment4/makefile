CXX ?= gcc
CXXFLAGS := -g -Wall -Wextra

all: user_level user_level_bad

user_level: user_level.c
	$(CXX) $(CXXFLAGS) user_level.c -o user_level

user_level_bad: user_level_bad.c
	$(CXX) $(CXXFLAGS) user_level_bad.c -o user_level_bad

clean:
	rm user_level user_level_bad
