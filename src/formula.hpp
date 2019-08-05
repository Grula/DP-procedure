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
  formula & getFormula(){return _f;}
  literalSet & getLits(){return _literals;}


private:
	// Unit Propagate
	bool _unitPropagate();
        bool _unitPropagate2();
	// Pure Literal
	bool _pureLiteral();
        bool _pureLiteral2();
	// Variable Elimination
        bool _resolution(clause &first, clause &second, literal p);
        clause& _resolution2(clause &first, clause &second, literal p);
	bool _eliminate(literal l);
        bool _eliminate2(literal l);

private:
	unsigned _nClause = 0,
			 _nLiteral = 0;
	formula _f;
	literalSet _literals;
	std::unordered_map<unsigned, std::pair<unsigned, unsigned>> _literalsCount;



};













#endif
