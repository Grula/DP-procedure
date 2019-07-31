#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <regex>
#include <vector>
#include <set>

#include "utils.hpp"

using literal = int;
using clause = std::set<literal>;
using formula = std::vector<clause>;

_ReadDIMACS::_ReadDIMACS()
	{};

_ReadDIMACS::_ReadDIMACS(std::string fileName, unsigned *nLiteral, unsigned *nClause, formula &f)
	{
    //std::regex confx("(\\s)*p(\\s)*cnf(\\s)*[0-9]+(\\s)*[0-9]+(\\s)*");
   // std::regex commentx("(\\s)*c(.*)");
    std::regex cnfclausx("(\\s)*(((-)?[0-9]+)\\s)+0");

		std::string line;
		_file.open(fileName);
		if (_file.is_open())
	  	{
	  		std::getline(_file, line);
	  		if(unlikely(!_splitAndCheck(line, nLiteral, nClause)))
	  		{
	  			*nLiteral = *nClause = 0;
	  		}
			while ( std::getline (_file,line) )
			{
                                //take only correct clauses
                                if (std::regex_match(line, cnfclausx)){
                                        clause c;
                                        std::cout << line << '\n';
                                        _fillClause(line, c);
                                        f.push_back(c);
                                    }
			}

			_file.close();
		}
	}

bool _ReadDIMACS::_splitAndCheck(const std::string& str, unsigned *nLiteral, unsigned *nClause)
{
	// ' ' - represents delimiter
	std::stringstream ss(str);
	std::string token;


	// checks if first letter is p
	std::getline(ss, token, ' ');
	if(token != "p") return false;

	// Check if given formula is in cnf
	std::getline(ss, token, ' ');
	if(token != "cnf") return false;

	// Checks if next tokens are numbers and saves them
	std::getline(ss, token, ' ');
	if(token.find_first_not_of( "0123456789" ) != std::string::npos) return false;
	*nLiteral = std::stoi(token);
	
	std::getline(ss, token, ' ');
	if(token.find_first_not_of( "0123456789" ) != std::string::npos) return false;
	*nClause = std::stoi(token);

	return true;
};

void _ReadDIMACS::_fillClause(const std::string &s, clause &c) 
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












