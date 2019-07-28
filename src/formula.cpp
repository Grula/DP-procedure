#include "formula.hpp"
#include "utils.hpp"

#include <vector>
#include <algorithm>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <iostream>

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

// DP procedure
bool Formula::DP()
{
	//TODO: if formula changed, we need to check unit prop and pure literal again
	do{
		_unitPropagate();
		/* TEST print */
		std::cout << "after unit prop\n"; 
		for(clause c : _f )
		{
			for(literal l : c)
			{
				std::cout << l << " ";
			}
			std::cout << std::endl;
		}
		std::cout << "***************\n"; 

	/********************/
		_pureLiteral();
		std::cout << "after pure lit\n"; 
		for(clause c : _f )
		{
			for(literal l : c)
			{
				std::cout << l << " ";
			}
			std::cout << std::endl;
		}
		std::cout << "***************\n";
	}while(false);
	for(literal l : _literals)
    {
        if(!_eliminate(l))
        {
            return false;
        }
    }
    return true;	
};

// Unit Propagate
void Formula::_unitPropagate()
{

	for(auto start = _f.begin(); start != _f.end(); start++)
	{
		if((*start).size() == 1)
		{
			// Remove other clauses
			_f.erase(
				std::remove_if(_f.begin(),_f.end(),
						  [&] (clause c) {
						  	return c.count(*(*start).begin()) > 0;
						  }),
				std::end(_f)
			);
			start = _f.begin();
		}
	}
};

// Pure Literal
void Formula::_pureLiteral()
{
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
		}


	}		
};
// Resolution
bool Formula::_resolution(clause &first, clause &second, literal p)
{
	auto itPosP = first.find(p);
	auto itPosNP = second.find(-p);

	// Erase p from frist clause and ~p from second clause
	first.erase(itPosP);
	second.erase(itPosNP);

	first.merge(second);
	// Search if exists q and ~q
	for(literal l : first)
	{
		auto it = first.find(-l);
		if(it != first.end())
			return false;
	}
	return true;
};

// Eliminate variable
bool Formula::_eliminate(literal l)
{
	for (size_t i = 0; i < _f.size(); i++)
	{
		for(size_t j = i + 1; j < _f.size(); j++)
		{
			auto containsC1 = _f[i].find(l);
			auto containsC2 = _f[j].find(-l);
			// Check if our clauses contains given literals
			if(containsC1 != _f[i].end() && containsC2 != _f[j].end())
			{
				// Check if given answer is tautology
				if(!_resolution(_f[i], _f[j], l))
				{
					auto c1IT = _f.begin() + i;
					_f.erase(c1IT);
				}
				// Erase second clause from formula
				auto c2IT = _f.begin() + (j - 1);
				_f.erase(c2IT);

				// Check if given clause is emppty
				if(_f[i].size() == 0)
				{
					return false;
				}
			}
		}
	}
	return true;
}
