#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <range/v3/view.hpp>
#include <range/v3/algorithm.hpp>

#include "formula.hpp"

#define log(X) ranges::for_each(X, [](auto s){ for (auto v: s) \
                    std::cout << v << ',';                     \
                   std::cout << '\n';        \
});

bool intersection2(auto rng1, auto rng2){
    bool exists;
    ranges::for_each(rng1, [&exists, &rng2](literal l){
        exists = ranges::count(rng2, l)>0;
        //std::cout << !exists << 'E';
        });
    return exists;
}




#include <chrono>
using namespace ranges::v3;
using namespace ranges;
int main(int argc, char const *argv[]){
	if(argc < 2)
		return 0;

        auto start = std::chrono::steady_clock::now();

        Formula f(argv[1]);

        if(f.DP())
	{
		std::cout << "-------------------" << std::endl;
		std::cout << "--- SATISFIABLE ---" << std::endl;
		std::cout << "-------------------" << std::endl;

	}
	else
	{
		std::cout << "---------------------" << std::endl;
		std::cout << "--- UNSATISFIABLE ---" << std::endl;
		std::cout << "---------------------" << std::endl;
	}

	auto end = std::chrono::steady_clock::now();
	std::cout << "Elapsed time in miliseconds : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
     f.print(std::cout);

	return 0;
}
