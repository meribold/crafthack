all: crafthack

crafthack: main.cpp
	g++ -std=c++2a -Wall -Wextra -pedantic main.cpp $$(pkg-config --libs glfw3 gl) -o crafthack

clean:
	rm -f crafthack
