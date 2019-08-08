#include "formula.hpp"
#include "utils.hpp"

#include <vector>
#include <algorithm>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <iostream>

//#define DEBUG

#define PRINT 1
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

    out << '}';
        }
    out << '}' << '\n';
    return out;
};


// DP procedure
bool Formula::DP()
{
	#ifdef DEBUG
	std::cout << "############## DEBUG PRINTING ##############" << std::endl;
	print(std::cout);
	std::cout << std::endl;
	std::cout << "############################################" << std::endl;
	#endif
	// size_t formulaSize;
	bool repeat = false;
	do{

		repeat = _unitPropagate() | _pureLiteral();
		// repeat = _unitPropagate();
		// repeat = _pureLiteral() || repeat;


    }while(repeat);
	for(literal l : _literals)
	{
        if(!_eliminate(l))
        {
            return false;
        }
	}

    return true;
}



// Unit Propagate
bool Formula::_unitPropagate()
{
	bool found_unit = false;
#ifdef PRINT
      std::cout << "Begin: Unit propagate\nFormula:";print(std::cout);
#endif

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
	#ifdef DEBUG
	std::cout << "############## DEBUG PRINTING ##############" << std::endl;
	std::cout << "Unit propagation: " << (found_unit ? ("True"):("False")) << std::endl;
	std::cout << "############################################" << std::endl;
	#endif

#ifdef PRINT
        print(std::cout);
      std::cout << "End: Unit propagate\n";
#endif



	return found_unit;


};

// Pure Literal
bool Formula::_pureLiteral()
{
#ifdef PRINT
      std::cout << "Begin: Pure literal\nFormula:";print(std::cout);
#endif


	bool found_pure = false;
	_literalsCount.clear(); // try to fill this just once
	// if(_literalsCount.size() == 0)
	// {
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
	// }
	for(auto start = _literalsCount.begin(); start != _literalsCount.end(); start++)
	{	
		int key = 0;
		if( (*start).second.first != 0 && (*start).second.second == 0)
		{
			key = (*start).first;
			// _literalsCount.erase(start);
		}
		else if ((*start).second.second != 0 && (*start).second.first == 0)
		{
			key = -(*start).first;
			// _literalsCount.erase(start);
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
	#ifdef DEBUG
	std::cout << "############## DEBUG PRINTING ##############" << std::endl;
	std::cout << "Pure literal: " << (found_pure ? ("True"):("False")) << std::endl;
	std::cout << "############################################" << std::endl;
	#endif
#ifdef PRINT
        print(std::cout);
        std::cout << "End: Unit propagate\n";
#endif


	return found_pure;	

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
					// erase second clause 
					_f.erase(_f.begin() + (j - 1));
					// Check if clause is empty ( containted only p and ~p)
					if(_f[i].size() == 0)
					{
						// Erase first caluse
						_f.erase(_f.begin() + i);
						// Empty clause - so formula is unsat
						return false;
					}
				}
				// Clause is tautology
				// delete first and second clause
				else 
				{
					#ifdef DEBUG
			    	std::cout << "############## DEBUG PRINTING ##############" << std::endl;
			    	std::cout << "############## _eliminate() ##############" << std::endl;
			    	std::cout << "Formula size : " <<_f.size() << std::endl;
			    	#endif

					_f.erase(_f.begin() + i);
					_f.erase(_f.begin() + (j - 1));
					
					#ifdef DEBUG
			    	std::cout <<  "Formula size : " << _f.size() << std::endl;
			    	std::cout << "############################################" << std::endl;
			    	#endif

				}				
			}
		}
	}
	return true;
}


