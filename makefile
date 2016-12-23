gt : main.o utils.o
	$(CXX) -o gt main.o utils.o

CXX = g++
CXXFLAGS = -std=c++11

main.o : main.cpp parameter.h utils.h

utils.o : utils.cpp utils.h parameter.h

clean :
	rm main.o utils.o gt
