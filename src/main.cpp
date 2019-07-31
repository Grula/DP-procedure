#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "formula.hpp"



int main(int argc, char const *argv[]){
	if(argc < 2)
		return 0;
	Formula f(argv[1]);

	if(f.DP())
	{
		std::cout << "Formula is SAT" << std::endl;
	}
	else
	{
		std::cout << "Formula is UNSAT" << std::endl;
	}

        f.print(std::cout);

	return 0;
}
