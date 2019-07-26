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
	//TODO: if formula changed, we need to check unit prop and pure literal again
	do{
		_unitPropagate();
		_pureLiteral();
	}while(false);

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

	return	_resolution();
};

// Unit Propagate
void Formula::_unitPropagate()
{
	for(auto start = _f.begin(); start != _f.end(); start++)
	{
		if((*start).size() == 1)
		{
			// Remove literal from other clauses
			auto l = (*start).begin();
			for(clause &c : _f)
			{
				auto it = c.find(*l);
				if(it != c.end())
					c.erase(it);
			}
			// Remove clause that has this one literal
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
					c.erase(it);
			}	
		}


	}		
};
// Resolution
bool Formula::_resolution()
{
	bool sat = true;
	for(auto first_clause = _f.begin(); first_clause != _f.end(); first_clause++)
	{
		for(literal l : *first_clause)
		{
			for(auto second_clause = first_clause+1; second_clause != _f.end(); second_clause++)
			{
				auto contains_literal = (*second_clause).find(-l);
				if(contains_literal != (*second_clause).end())
				{
					(*first_clause).erase((*first_clause).find(l));
					(*second_clause).erase(contains_literal);
					clause tmp;
					std::merge((*first_clause).begin(),(*first_clause).end(),
							   (*second_clause).begin(),(*second_clause).end(),
							   std::inserter(tmp, tmp.begin()));
					if(tmp.size() == 0)
					{
						sat = false;
						break;
					}
					for(literal l : tmp)
					{
						std::cout << l << " ";
					}
					std::cout << std::endl;
					_f.push_back(tmp);
					_f.erase(first_clause);
					_f.erase(second_clause);
				}
			}
			if(!sat)
				break;
		}
		if(!sat)
			break;

	}
	return sat;
};
