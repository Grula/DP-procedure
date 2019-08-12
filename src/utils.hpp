#ifndef _DP_UTILS__
#define _DP_UTILS__

#include <vector>
#include <unordered_set>
#include <set>
#include <fstream>
#include <algorithm>
#include <sstream>

#define likely(X) __builtin_expect((X),1) 
#define unlikely(X) __builtin_expect((X),0) 

using literal = int;
using clause = std::set<literal>;
using formula = std::vector<clause>;

/*
 *	Class which reads given DIMACS file and saves
 *  content in given data structures
 */
class _ReadDIMACS
{
public:
	_ReadDIMACS();
	_ReadDIMACS(std::string fileName, unsigned *nLiteral, unsigned *nClause, formula &f);
private:
	std::ifstream _file;
	/*
	 * Checking first line of DIMACS file
	 */
	bool _splitAndCheck(const std::string& str, unsigned *nLiteral, unsigned *nClause);
        void _fillClause( std::string &s, clause &c);
};







#endif
