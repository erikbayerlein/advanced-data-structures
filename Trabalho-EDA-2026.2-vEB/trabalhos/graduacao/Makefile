CXX ?= g++
CXXFLAGS ?= -O2 -Wall -Wextra -Wpedantic
INPUT ?= entrada.in
export INPUT

# Suprime os avisos de diretório nas chamadas ao Make que herdam MAKEFLAGS.
MAKEFLAGS += --no-print-directory

.PHONY: build run test

build: programa
	@:

programa: main.cpp van_emde_boas.cpp van_emde_boas.hpp cluster_table.hpp
	@$(CXX) -std=c++17 $(CPPFLAGS) $(CXXFLAGS) main.cpp van_emde_boas.cpp $(LDFLAGS) $(LDLIBS) -o $@

run: build
	@./programa "$$INPUT"

test_veb: test_veb.cpp van_emde_boas.cpp van_emde_boas.hpp cluster_table.hpp
	@$(CXX) -std=c++17 $(CPPFLAGS) $(CXXFLAGS) -UNDEBUG test_veb.cpp van_emde_boas.cpp $(LDFLAGS) $(LDLIBS) -o $@

test: build test_veb
	@sh test_cli.sh
	@./test_veb
