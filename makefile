crafthack: main.cpp
	g++ -std=c++2a -Wall -Wextra -Wold-style-cast -Wconversion -pedantic main.cpp $$(pkg-config --libs glfw3 gl glew libpng) -o crafthack

clean:
	rm -f crafthack
