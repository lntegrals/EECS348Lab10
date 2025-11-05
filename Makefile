## Makefile for EECS 348 Lab 10 – String‑based calculator

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -O2

# Target executable name
TARGET := calculator

# Source files
SRCS := main.cpp string_calculator.cpp

# Object files derived from source files
OBJS := $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp string_calculator.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean