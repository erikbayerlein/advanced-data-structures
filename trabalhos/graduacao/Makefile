CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic

export INPUT

.PHONY: build run

build: programa

programa: main.cpp persistent_list.cpp persistent_list.hpp
	@$(CXX) $(CXXFLAGS) main.cpp persistent_list.cpp -o $@

run: build
	@./programa "$$INPUT"
