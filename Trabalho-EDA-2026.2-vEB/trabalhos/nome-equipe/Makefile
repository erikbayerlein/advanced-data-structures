INPUT ?= entrada.in

.PHONY: build run clean

build:
	g++ -std=c++17 -Wall -Wextra -O2 -o programa main.cpp

run: build
	./programa $(INPUT)

clean:
	rm -f programa
