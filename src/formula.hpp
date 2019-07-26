#ifndef _DP_FORMULA__
#define _DP_FORMULA__

#include "utils.hpp"
#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>


class Formula
{
public:
	Formula();
	Formula(std::string fileName);

	// DP procedure
	bool DP();

private:
	// Unit Propagate
	void _unitPropagate();
	// Pure Literal
	void _pureLiteral();
	// Variable Elimination
	bool _resolution();

private:
	unsigned _nClause = 0,
			 _nLiteral = 0;
	formula _f;
	std::unordered_map<unsigned, std::pair<unsigned, unsigned>> _literalsCount;


};













#endif