#include "formula.hpp"
#include "utils.hpp"

#include <vector>
#include <algorithm>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <iostream>

#include <range/v3/all.hpp>

using namespace ranges::v3;
using namespace ranges;

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

Formula::Formula()
{};

Formula::Formula(std::string fileName)
{
	_ReadDIMACS(fileName, &_nLiteral, &_nClause, _f);
	_literalsCount.reserve(_nLiteral);

	_literals.reserve(_nLiteral);
	for(clause c : _f)
	{
		for(literal l : c)
		{
			_literals.insert(l);
		}
	}

};

std::ostream &Formula::print(std::ostream &out) const
{
    out << '{';
    for(clause c: _f){
    out << '{';
    auto lit = c.cbegin();
    auto litend = c.cend();

    while(lit!=litend){
            out << (*lit);
          if (std::prev(litend) != lit)
              out << ',';

            lit++;
        }

    out << '}' << '\n';
        }
    out << '}';
    return out;
};


// DP procedure
bool Formula::DP()
{
	// size_t formulaSize;
	bool repeat = false;
	do{
                repeat = _unitPropagate2() | _pureLiteral2();
		// repeat = _unitPropagate();
		// repeat = _pureLiteral() || repeat;
    }while(false);
        /*for(literal l : _literals)
    {
        if(unlikely(!_eliminate(l)))
        {
            return false;
        }
    }
*/
        return true;
}



// Unit Propagate
bool Formula::_unitPropagate()
{
	bool found_unit = false;

	for(auto start = _f.begin(); start != _f.end(); start++)
	{
		if((*start).size() == 1)
		{
			// Get single literal
			literal l = *(*start).begin();
			// Remove other clauses that contain given literal
			_f.erase(
				std::remove_if(_f.begin(),_f.end(),
						  	  [=] (clause c) {
						  	  	return c.count(l) > 0;
						  	  }
						    ),
				std::end(_f)
			);

			// Remove oposite literal from clauses that contains it
			for(auto rStart = _f.begin(); rStart != _f.end(); rStart++)
			{
				auto opositeLiteral = (*rStart).find(-l);
				if( opositeLiteral != (*rStart).end() )
				{
					(*rStart).erase(opositeLiteral);
				}
			}
			found_unit = true;
			start = _f.begin();
		}
	}
	// std::cout << "Unit prop " << found_unit << std::endl;

        return found_unit;
}

bool Formula::_pureLiteral2()
{
    //uzimamo samo pozitivne literale
    auto pos = _literals | view::filter([](literal l){return l>0;});

    auto neg = _literals
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
    log(_f);


    auto rngf = _f | view::filter([&pure](auto s){
            return !intersection2(pure, s);
        })
            | to_vector;

    if(pure){
        std::cout << "After pure literal!" << std::endl;
        _f.clear();
        _f = rngf;
        log(_f);
        return true;
    }else
        return false;

};

// Pure Literal
bool Formula::_pureLiteral()
{
	bool found_pure = false;
	_literalsCount.clear(); // try to fill this just once
	for(clause c : _f )
	{	
		for(literal l : c)
		{
			if(_literalsCount.count(std::abs(l)) == 0)
			{
				if( l > 0)
				{
					_literalsCount.insert({std::abs(l), {1,0}});
				}
				else
				{
					_literalsCount.insert({std::abs(l), {0,1}});
				}

			}
			else
			{
				// Auto - iterator
				auto pairs = _literalsCount.find(std::abs(l));
				if(l > 0)
				{
					(*pairs).second.first++;
				}
				else
				{
					(*pairs).second.second++;
				}
			}
		}	

	}
	// for(clause c : _f)
	// {
	// 	for(literal l : c)
	// 	{
	// 		std::cout << l << " ";
	// 	}
	// 	std::cout << std::endl;
	// }
	// std::cout << "edning f" << std::endl;
	for(auto start = _literalsCount.begin(); start != _literalsCount.end(); start++)
	{	
		int key = 0;
		if( (*start).second.first != 0 && (*start).second.second == 0)
		{
			key = (*start).first;
		}
		else if ((*start).second.second != 0 && (*start).second.first == 0)
		{
			key = -(*start).first;
		}

		if(key != 0)
		{
			_f.erase(
				std::remove_if(_f.begin(),_f.end(),
						  [=] (clause c) {
						  	return c.count(key) > 0;
						  }),
				std::end(_f)
			);
			found_pure = true;
		}
	}
	// std::cout << "Pure lit " << found_pure << std::endl;

	return found_pure;	

}

bool Formula::_unitPropagate2()
{
    auto units = _f | view::transform([](auto s){
            if (s.size()==1)
                return *(s.cbegin());//ovo probati bolje

        })
            | view::filter([](auto value){return value != 0;});

    std::cout << units;


    log(_f);
    auto Fp = _f | view::filter([&units](auto clause){
            return !intersection2(clause, units);
        });

    /*auto intersection = [](auto rng1, auto rng2){
            bool exists;
            ranges::for_each(rng1, [&exists, &rng2](literal l){
                exists = ranges::count(rng2, l)>0;
               // std::cout << !exists << 'E';
                });
            return exists;

        };
*/

        //log(Fp);

       //sad izbacujemo svaki element iz unitsa samo sa suprotnim znakom
       auto neg_units = units | view::transform([](literal unit){return -unit;});
       //std::cout << neg_units;

       //ovaj deo ovde ima problem
       auto Fpp = Fp
               | view::transform([&neg_units](std::set<literal> s){

               auto ts = s
               | view::transform([&neg_units](literal l){
               if(ranges::count(neg_units, l) > 0)
                    return 0;
               else
                    return l;
               })
               | view::filter([](literal l){return l!=0;})
               ;
               return ts | to_vector;
               })
               | to_vector;


        if(units){
           std::cout << "Formula after unit propagation:\n";
           log(Fpp);//vektor vektor int

       /* _f.clear();
        //da se kastuje vektor vektor intova u vektor skup intova
        for(std::vector<int> v : Fpp)
            {   std::set<int> s(v.begin, v.end());
                _f.insert(s);
            }
*/
           return true;
            }
       else
            return false;
       //   std::cout << 'R' << ranges::size(Fpp)<< 'R';
        //kastujem u vektor skupova

};
// Resolution
// Returs true if clause after resolution is tautology
bool Formula::_resolution(clause &first, clause &second, literal p)
{
	// Erase p from frist clause and ~p from second clause
	first.erase(first.find(p));
	second.erase(second.find(-p));

	first.merge(second);
	// Search if exists q and ~q
	for(literal l : first)
	{
		if(first.find(-l) != first.end())
			return true;
	}
	return false;
};

// Eliminate variable
bool Formula::_eliminate(literal l)
{
	for (size_t i = 0; i < _f.size(); i++)
	{
		for(size_t j = i + 1; j < _f.size(); j++)
		{
			// Check if our clauses contains given literals
			if(_f[i].find(l) != _f[i].end() && _f[j].find(-l) != _f[j].end())
			{

				// Check if given answer is not tautology
				if(!_resolution(_f[i], _f[j], l))
				{
					_f.erase(_f.begin() + (j - 1));
					// Check if clause is empty
					if(_f[i].size() == 0)
					{
						// Erase first caluse and return false
						_f.erase(_f.begin() + i);
						return false;
					}
				}
				// Clause is tautology so delete first and second clause
				else 
				{
					_f.erase(_f.begin() + i);
					_f.erase(_f.begin() + (j - 1));
				}
				// for(literal l : _f[i])
				// {
				// 	std::cout << l << " ";
				// }				
			}
		}
	}
	return true;
}


