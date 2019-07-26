#include "formula.hpp"
#include "utils.hpp"

#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>

Formula::Formula()
{};

Formula::Formula(std::string fileName)
{
	_ReadDIMACS(fileName, &_nLiteral, &_nClause, _f);
	_literalsCount.reserve(_nLiteral);
};

// DP procedure
bool Formula::DP()
{
	_unitPropagate();
	_pureLiteral();

	/* TEST print */
	for(clause c : _f )
	{
		for(literal l : c)
		{
			std::cout << l << " ";
		}
		std::cout << std::endl;
	}
	/********************/

	return true;
};

// Unit Propagate
void Formula::_unitPropagate()
{
	for(auto start = _f.begin(); start != _f.end(); start++)
		{
		if((*start).size() == 1)
		{
			_f.erase(start);
			start--;
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
			// Take ref so we modify real clause
			for(clause &c : _f)
			{
				auto it = c.find(key);
				if(it != c.end())
					c.erase(it); // TODO: fix bug
			}	
		}


	}		
};
// Variable Elimination
void Formula::_variableElimination() {};
