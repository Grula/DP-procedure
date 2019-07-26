#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

#include <vector>
#include <set>
#include <unordered_map>

#include "formula.hpp"



int main(int argc, char const *argv[]){
	
	Formula f("test/sample_test.txt");
	
	if(f.DP())
	{
		std::cout << "Formula is SAT" << std::endl;
	}
	else
	{
		std::cout << "Formula is UNSAT" << std::endl;
	}


	return 0;
}