CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic

export INPUT

.PHONY: build run

build: programa

programa: main.cpp
	@$(CXX) $(CXXFLAGS) $< -o $@

run: build
	@./programa "$$INPUT"
