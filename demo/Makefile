all: main

main: main.cpp
	g++ main.cpp -L../build -lengine -lGL -lGLEW -lglfw -lassimp -o demo

run: demo
	LD_LIBRARY_PATH=../build ./demo

clean:
	rm demo
