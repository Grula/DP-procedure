#ifndef _DP_FORMULA__
#define _DP_FORMULA__

#include "utils.hpp"
#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <iostream>

using literalSet = std::unordered_set<literal>;


class Formula
{
public:
	Formula();
	Formula(std::string fileName);

	// DP procedure
	bool DP();
	std::ostream & print(std::ostream & out)const;
private:
	// Unit Propagate
	bool _unitPropagate();
	// Pure Literal
	bool _pureLiteral();
	// Variable Elimination
	bool _resolution(const clause &first,const clause &second,const literal p);

	bool _eliminate(literal l);

private:
	unsigned _nClause = 0,
			 _nLiteral = 0;
	formula _f;
	literalSet _literals;
	std::unordered_map<unsigned, std::pair<unsigned, unsigned>> _literalsCount;




};













#endif
