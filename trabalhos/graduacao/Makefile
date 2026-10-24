CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic

export INPUT

# Com `make -C pasta run`, o make imprimiria "Entering directory" no stdout,
# misturado às respostas comparadas byte a byte.
MAKEFLAGS += --no-print-directory

.PHONY: build run test

build: programa
	@:

programa: main.cpp persistent_list.cpp persistent_list.hpp
	@$(CXX) $(CXXFLAGS) main.cpp persistent_list.cpp -o $@

run: build
	@./programa "$$INPUT"

test:
	@sh test_regression.sh
