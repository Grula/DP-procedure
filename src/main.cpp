#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

#include <vector>
#include <set>
#include <unordered_map>




using literal = int;
using clause = std::set<literal>;
using formula = std::vector<clause>;


void print(std::string s)
{
	std::cout <<  s << std::endl;
}


/*
 *	Move to UTILITIES and give description
 *
 *
 *
 */
template <class Container>
class _ReadDIMACS
{
public:
	_ReadDIMACS(){}
	_ReadDIMACS(std::string fileName, unsigned *nLiteral, unsigned *nClause, formula &f)
	{
		std::string line;
		_file.open(fileName);
		if (_file.is_open())
	  	{	
	  		print("...Opening file...");	
	  		std::getline(_file, line);
	  		if(!splitAndCheck(line, nLiteral, nClause))
	  		{
	  			*nLiteral = *nClause = 0;
	  		}
			while ( std::getline (_file,line) )
			{
				clause c;
				fillClause(line, c);
				f.push_back(c);
			}

			_file.close();
		}
	}

private:
	std::ifstream _file;


	/*
	 * Checking first line of DIMACS file
	 */
	bool splitAndCheck(const std::string& str, unsigned *nLiteral, unsigned *nClause)
	{
		// ' ' - represents delimiter
		std::stringstream ss(str);
		std::string token;


		//
		std::getline(ss, token, ' ');
		if(token != "p")
		{
			return false;
		}
		// Check if given formula is in _f
		std::getline(ss, token, ' ');
		if(token != "_f")
		{
			return false;
		}


		// Take number of given Literals and Clauses
		std::getline(ss, token, ' ');
		if(token.find_first_not_of( "0123456789" ) != std::string::npos)
		{
			return false;
		}
		*nLiteral = std::stoi(token);
		
		std::getline(ss, token, ' ');
		if(token.find_first_not_of( "0123456789" ) != std::string::npos)
		{
			return false;
		}
		*nClause = std::stoi(token);

		return true;
	};

	void fillClause(const std::string &s, clause &c) 
	{
		std::stringstream ss(s);
		std::string literal;

		while(std::getline(ss, literal, ' '))
		{
			int l = std::stoi(literal);
			if( l == 0)
				break;
			c.insert(l);
		}
	};

};

/**************************************/

class Formula
{
public:
	Formula()
	{}

	Formula(std::string fileName)
	{
		_ReadDIMACS<formula>(fileName, &_nLiteral, &_nClause, _f);
		_literalsCount.reserve(_nLiteral);
	}

	// DP procedure
	bool DP()
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

private:
	// Unit Propagate
	void _unitPropagate()
	{
		for(auto start = _f.begin(); start != _f.end(); start++)
			{
			if((*start).size() == 1)
			{
				_f.erase(start);
				print("Found single literal");
				start--;
			}
		}
	};

	// Pure Literal
	void _pureLiteral()
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

			// If 0 means its not pure
			print("key:    " + std::to_string(key));
			print("vals_+: " + std::to_string((*start).second.first));
			print("vals_-: " + std::to_string((*start).second.second));
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
	void _variableElimination() {};

private:
	unsigned _nClause = 0,
			 _nLiteral = 0;
	formula _f;
	std::unordered_map<unsigned, std::pair<unsigned, unsigned>> _literalsCount;


};



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