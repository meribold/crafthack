all: crafthack

crafthack: main.cpp
	g++ -std=c++2a -Wall -Wextra -pedantic main.cpp $$(pkg-config --libs glfw3) -o crafthack

clean:
	rm -f crafthack
