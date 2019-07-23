#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

#include <vector>
#include <set>
#include <unordered_map>




using literal = int;
using clause = std::set<literal>;

std::vector<clause> CNF;


bool splitAndCheck(const std::string& str,  unsigned &nLiteral, unsigned &nClause, char delim = ' ')
{
    std::stringstream ss(str);
    std::string token;

    // First arg P
    std::getline(ss, token, delim);
    if(token != "p")
    	return false;
    
    // Check if formula is CNF
    std::getline(ss, token, delim);
    if(token != "cnf")
    	return false;

    // Take next arguments
    std::getline(ss, token, delim);
    if(token.find_first_not_of( "0123456789" ) != std::string::npos)
    	return false;
	nLiteral = std::stoi(token);
   
    std::getline(ss, token, delim);
    if(token.find_first_not_of( "0123456789" ) != std::string::npos)
    	return false;
	nClause = std::stoi(token);

    return true;
}


clause &fillClause(const std::string &s) 
{
	clause *c = new clause;
	std::stringstream ss(s);
    std::string literal;

    while(std::getline(ss, literal, ' '))
    {
    	int l = std::stoi(literal);
    	if( l == 0)
    		break;
    	(*c).insert(l);
    }
    return *c;
}

int main(int argc, char const *argv[]){
	
	unsigned nClause = 0,
			 nLiteral = 0;
	std::string line;
	std::ifstream myfile ("filename.txt");
	

	if (myfile.is_open())
  	{	
  		std::getline(myfile, line);
  		if(!splitAndCheck(line, nLiteral, nClause))
  			return false;
	    
	    while ( std::getline (myfile,line) )
	    {
	    	CNF.push_back(fillClause(line));
	    	// std::cout << line << '\n';

	    }
    	myfile.close();
	}



	// Unit Propagate
	for(auto start = CNF.begin(); start != CNF.end(); start++)
	{
		if((*start).size() == 1)
		{
			CNF.erase(start);
			// std::cout << "Found 1" << std::endl;
			start--;
		}

	}
	// for(clause c : CNF )
	// 	for(literal l : c)
	// 		std::cout << l << " ";

	// Pure Literal
	std::unordered_map<unsigned, std::pair<unsigned, unsigned>> literalsCount;
	// literalsCount.reserve(nLiteral);
	for(clause c : CNF )
	{	
		for(literal l : c)
		{
			if(literalsCount.count(std::abs(l)) == 0){
				if( l > 0)
					literalsCount.insert({std::abs(l), {1,0}});
				else
					literalsCount.insert({std::abs(l), {0,1}});

			}
			else
			{
				auto pairs = literalsCount.find(std::abs(l));

				if(l > 0)
				{
					// unsigned t = pairs.first;
					(*pairs).second.first++;
					// std::cout << (*pairs).second.first << std::endl;
				}
				else
				{

					(*pairs).second.second++;
					// std::cout << (*pairs).second.second << std::endl;	
				}
			}
		}	

	}

	for(auto start = literalsCount.begin(); start != literalsCount.end(); start++)
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
			for(clause c : CNF)
			{
				auto it = c.find(key);
				if(it != c.end())
					c.erase(it);
			}	
		}

		for(clause c : CNF )
			for(literal l : c)
				std::cout << l << " ";
			std::cout << std::endl;
		// std::cout <<"key: " << (*start).first << std::endl;
		// std::cout <<"vals_+: " << (*start).second.first << std::endl;
		// std::cout <<"vals_-: " << (*start).second.second << std::endl;
	}
	// Variable Elimination

	// for(clause c : CNF )
	// 	for(literal l : c)
	// 		std::cout << l << " ";





	return 0;
}