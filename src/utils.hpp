#ifndef _DP_UTILS__
#define _DP_UTILS__

#include <vector>
#include <set>
#include <fstream>
#include <algorithm>
#include <sstream>

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
	bool splitAndCheck(const std::string& str, unsigned *nLiteral, unsigned *nClause);
	void fillClause(const std::string &s, clause &c);
};







#endif