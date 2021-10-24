CXXFLAGS := -std=c++20 -Wall -Wextra -Wold-style-cast -Wconversion -pedantic -O3

crafthack: main.o block.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $+ $$(pkg-config --libs glfw3 gl glew libpng) -o $@

main.o: main.cpp block.hpp
block.o: block.hpp block.cpp

%.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $*.cpp -o $@

clean:
	rm -f block.o main.o crafthack
