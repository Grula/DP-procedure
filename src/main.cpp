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
        std::cout << !exists << 'E';
        });
    return exists;
}




#include <chrono>
using namespace ranges::v3;
using namespace ranges;
int main(int argc, char const *argv[]){
	if(argc < 2)
		return 0;

	//auto start = std::chrono::steady_clock::now();

	Formula f(argv[1]);
  //std::vector<std::set<int>>
  formula ff = f.getFormula();
  literalSet literals;
  //std::cout << ff.at(1).size();
//pure literal
    literals = f.getLits();

    for(auto l: literals)
        std::cout << l << ' ';
    //uzimamo samo pozitivne literale
    auto pos = literals | view::filter([](literal l){return l>0;});

    auto neg = literals
               | view::filter([](literal l){return l<0;})
               | view::transform([&pos](literal l){
        return std::abs(l);});

    auto pos_only = pos
            | view::filter([&neg](literal l){
                return ranges::none_of(neg, [&](literal v){return l==v;});
        });
  
    auto neg_only = neg
        | view::filter([&pos](literal l){
            return ranges::none_of(pos, [&](literal v){return l==v;});
    });


    auto pure = view::concat(pos_only,

                             neg_only
                             | view::transform([](literal l){return -l;;})
                             );
    std::cout << pure;

    std::cout << std::endl;
    log(ff);
    //drugacije nekako presek
    auto rngf = ff | view::filter([&pure](auto s){

            return false;
        });

    log(rngf);

    //odavde krece unit prop
    std::cout << "\n\n\n[unit prop]\n\n\n\n\n";
    auto units = ff | view::transform([](auto s){
            if (s.size()==1)
                return *(s.cbegin());

        })
            | view::filter([](auto value){return value != 0;});

    //std::cout << units << "a----" << intersection2(units);

    //skloni sve klauze koje imaju element iz jedinicne klauze

    log(ff);
    auto Fp = ff | view::filter([&units](auto clause){
            return !intersection2(clause, units);
        });

    auto intersection = [](auto rng1, auto rng2){
            bool exists;
            ranges::for_each(rng1, [&exists, &rng2](literal l){
                exists = ranges::count(rng2, l)>0;
               // std::cout << !exists << 'E';
                });
            return exists;

        };
    std::cout << '\n';
        log(Fp);

       //sad izbacujemo svaki element iz unitsa samo sa suprotnim znakom
       auto neg_units = units | view::transform([](literal unit){return -unit;});
    //   std::cout << neg_units;



















  /*
   *
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
    std::cout<< std::endl;
    */
	return 0;
}
