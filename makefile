all: crafthack

crafthack: main.cpp
	g++ main.cpp $$(pkg-config --libs glfw3) -o crafthack

clean:
	rm -f crafthack
