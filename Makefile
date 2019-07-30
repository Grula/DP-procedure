PROGRAM = main
CXX = g++
CXXFLAGS = -Wall -g -std=c++17

$(PROGRAM): src/main.o  src/formula.o src/utils.o
	$(CXX) -o $(PROGRAM) src/main.o  src/formula.o src/utils.o

main.o : src/main.cpp src/utils.hpp src/formula.hpp
	$(CXX) -c src/main.cpp

formula.o : src/formula.o src/formula.hpp src/utils.hpp
	$(CXX) -c formula.cpp utils.cpp

utils.o : src/utils.cpp src/utils.hpp
	$(CXX) -c utils.cpp

.PHONY: clean run

clean:
	rm -rf src/main.o src/utils.o src/formula.o main

run:
	make clean && make && ./main
