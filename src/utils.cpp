#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <regex>
#include <vector>
#include <set>
#include <string>
#include "utils.hpp"


_ReadDIMACS::_ReadDIMACS()
	{};

_ReadDIMACS::_ReadDIMACS(std::string fileName, unsigned *nLiteral, unsigned *nClause, formula &f)
        {

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
                                        clause c;
                        //                std::cout << line << '\n';
                                        _fillClause(line, c);
                                        f.push_back(c);
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

void _ReadDIMACS::_fillClause(std::string &s, clause &c)
{
        std::regex litexp("[-]?(\\d)+");

        std::regex_token_iterator<std::string::iterator> rti ( s.begin(), s.end(), litexp);
        std::regex_token_iterator<std::string::iterator> rend;
        while (rti!=rend){
              int l = std::stoi(*rti);

              if(l!=0)

                  {
                      //std::cout << '[' << l << ']';
                      c.insert(l);
                  }
            rti++;
            }
/*
	while(std::getline(ss, literal, ' '))
	{
		int l = std::stoi(literal);
		if( l == 0)
			break;
		c.insert(l);
	}
*/
};
