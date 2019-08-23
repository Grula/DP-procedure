#include "formula.hpp"
#include "utils.hpp"

#include <vector>
#include <algorithm>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <iostream>

// #define DEBUG

#define wcls(X) std::cout << '{'; for(literal l: X) std::cout << l << ' '; std::cout << "}\n";
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
		std::cout << "*****************Printing Formula*****************" << std::endl;
		print(std::cout);
		std::cout << std::endl;
	#endif
	// size_t formulaSize;
	bool repeat = false;
	do{
		repeat = _unitPropagate() | _pureLiteral();
	}while(repeat);


	for(literal l : _literals)
	{
		if(!_eliminate(l))
		{
			return false;
		}
		// If formula is empty - return SAT
		if(_f.size()==0) return true;//if vector is empty
		do{
			repeat = _unitPropagate() | _pureLiteral();
		}while(repeat);
	}

	return true;
}



// Unit Propagate
bool Formula::_unitPropagate()
{
	bool found_unit = false;
	#ifdef DEBUG
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
						  	  [=] (clause &c) {
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
	std::cout << "Unit propagation: " << (found_unit ? ("True"):("False")) << std::endl;print(std::cout);
	#endif

	return found_unit;


};

// Pure Literal
bool Formula::_pureLiteral()
{


	bool found_pure = false;
	//TODO : Fill literals count only once, this is slowing down a lot
	_literalsCount.clear();

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
		std::cout << "Pure literal: " << (found_pure ? ("True"):("False")) << std::endl;
	#endif



	return found_pure;	

};


/* Borrowed from stackOverflow
 * URL : https://stackoverflow.com/questions/24263259/c-stdseterase-with-stdremove-if
 */
template <class T, class Comp, class Alloc, class Predicate>
void discard_if(std::set<T, Comp, Alloc>& c, Predicate pred) {
    for (auto it{c.begin()}, end{c.end()}; it != end; ) {
        if (pred(*it)) {
            it = c.erase(it);
        }
        else {
            ++it;
        }
    }
}


// Resolution
// Returs true if clause after resolution is tautology
bool Formula::_resolution(const clause &first,const clause &second,const literal p)
{
	#ifdef DEBUG
	    std::cout << "################\n";
	    std::cout << "Resolution begin.\n";
	    std::cout << "First:";
	    wcls(first);
	    std::cout << "Second:";
	    wcls(second);
	#endif

    clause c;
	c.insert(first.begin(), first.end());
	c.insert(second.begin(), second.end());
    auto containsp = [p](literal l){return l==p || l==(-p);};
    #ifdef DEBUG
        std::cout << "Before removing:";wcls(c); std::cout << '\n';
    #endif
    discard_if(c, containsp);
    #ifdef DEBUG
        std::cout << "After removing:";wcls(c); std::cout << '\n';
    #endif

    for(literal l : c)
	{
    	if(c.find(-l) != c.end())
    	{
		    #ifdef DEBUG
		        std::cout << "Found tautology! \n";
		    #endif
			return true;
    	}
    }
    std::cout << std::endl;
    #ifdef DEBUG
        std::cout << "Before adding clause"; print(std::cout);
		std::cout << "################\n";
    #endif

    _f.push_back(c);
	return false;
};

// Eliminate variable
bool Formula::_eliminate(literal l)
{
	#ifdef DEBUG
	    std::cout << "*************************************************" << '\n';
	    std::cout << "Elimination for variable:" << l << '\n';
	#endif
	// std::vector<std::vector<clause>::iterator> toErase;
	auto itLast = _f.end()-1;
    	

	// NOTICE: 
	//		 - For some reason iteraot itLast becomes invalid, making workaround
	size_t lastElem = _f.size();
	for(size_t i = 0; i < lastElem; i++)
	{
		if(_f[i].size() == 0) return false;
		for(size_t j = i+1; j < lastElem; j++)
		{
			if(_f[j].size() == 0) return false;
			if(_f[i].find(l) != _f[i].end() && _f[j].find(-l) != _f[j].end())
			{

				if(!_resolution(_f[i], _f[j], l))
				{
					// Check if last added clause is empty
					if(!(std::end(_f)-1)->size())
						return false;
				}
				#ifdef DEBUG
					std::cout << "After resolution:";
					print(std::cout);
					std::cout << "Formula size : " <<_f.size() << '\n';
				#endif

			}
		}
	}
	/*
    for(auto itFirst = _f.begin(); itFirst < itLast; itFirst++)
	{
			std::cout << "LAST CLAUSE" << std::endl;
			for(auto c: *itLast)
			{
				std::cout << c << " ";
			}
			std::cout  << std::endl;
			std::cout  << std::endl;

		std::cout << "First c" << std::endl;
		for(auto c: *itFirst)
		{
			std::cout << c << " ";
		}
		std::cout  << std::endl;
		std::cout  << std::endl;

        for(auto itSecond = itFirst+1; itSecond < itLast+1; itSecond++)
		{
			std::cout << "Size : " <<  (*itSecond).size() << '\n';
			std::cout << "Second : ";
			for(auto c: *itSecond)
			{
				std::cout << c << " ";
			}
			std::cout  << std::endl;

			if((*itFirst).find(l) != (*itFirst).end() && (*itSecond).find(-l) != (*itSecond).end())
			{

				if(!_resolution(*itFirst, *itSecond, l))
				{
					// Check if last added clause is empty
					if(!(std::end(_f)-1)->size())
						return false;
				}
				#ifdef DEBUG
					std::cout << "After resolution:";
					print(std::cout);
					std::cout << "Formula size : " <<_f.size() << '\n';
				#endif
				// toErase.push_back(itFirst);
				// toErase.push_back(itSecond);
			}
		}

	}	
	*/
	#ifdef DEBUG
	    std::cout << "Variable:" << l << " eliminated" <<'\n';
	    std::cout << "*************************************************" << '\n';
	#endif

	// for(size_t i = 0; i < toErase.size(); i++)
	// {
	// 	_f.erase(toErase[i]);
	// }
        // std::cout << "After removing elements:\n";
        // print(std::cout);
	return true;
}


