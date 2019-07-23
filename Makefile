PROGRAM = main
CXX = g++
CXXFLAGS = -Wall -g

$(PROGRAM): src/main.o
	$(CXX) -o $(PROGRAM) src/main.o


.PHONY: clean run

clean:
	rm -rf src/main.o main

run:
	make clean && make && ./main
