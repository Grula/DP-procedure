#include "formula.hpp"
#include "utils.hpp"

#include <vector>
#include <algorithm>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <iostream>

#include <range/v3/view.hpp>
#include <range/v3/algorithm.hpp>
#include <range/v3/action.hpp>






using namespace ranges::v3;
using namespace ranges;

#define log(X) ranges::for_each(X, [](auto s){ for (auto v: s) \
                    std::cout << v << ',';                     \
                   std::cout << '\n';        \
});

#define wcls(X) std::cout << '{'; for(auto val: X) std::cout  << val << ' '; std::cout << '}';


#define DEBUG 1

bool intersection2(auto rng1,auto rng2){
    bool exists = false;

    for(literal l: rng1){
        if(ranges::find(rng2, l) != ranges::end(rng2))
            {exists = true; break;}
        }

    return exists;

}

Formula::Formula()
{};

Formula::Formula(std::string fileName)
{
	_ReadDIMACS(fileName, &_nLiteral, &_nClause, _f);
	_literalsCount.reserve(_nLiteral);
        _literals.reserve(_nLiteral);
        //std::cout << _nLiteral;
        //_literals2.reserve(_nLiteral);
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

    _getLiterals();bool repeat;
    do {repeat = _unitPropagate2() || _pureLiteral2();
    }while(repeat);

        //ako je _f prazna vrati true
         if(_f.size()==0)
             return true;

            _getLiterals();
         for(literal l: _literals){
            //ako eliminate izvede praznu klauzu, vratiti unsat
                bool isClauseEmpty = _eliminate2(l);
                if(isClauseEmpty)
                    return false;
                if(_f.size()==0)
                    return true;
             }
         return true;

}


bool Formula::_pureLiteral2()
{
#ifdef DEBUG
    std::cout << "Begin:Pure literal\n";
#endif

    auto pos = _literals | view::filter([](literal l){return l>0;});

    auto pos2 = _f | view::join | to_vector | action::sort | action::unique;

    auto pos3 = pos2 |  view::filter([](literal l){return l>0;});

    //std::cout <<pos3;

    auto neg = _literals
               | view::filter([](literal l){return l<0;})
               | view::transform([&pos](literal l){
        return std::abs(l);});

    auto neg2 = _f | view::join | to_vector | action::sort | action::unique;

    auto neg3 = neg2 |  view::filter([](literal l){return l<0;})
            | view::transform([](literal l){return std::abs(l);});


    auto pos_only = pos3
            | view::filter([&neg3](literal l){
                return ranges::none_of(neg3, [&](literal v){return l==v;});
        });

    auto neg_only = neg3
        | view::filter([&pos3](literal l){
            return ranges::none_of(pos3, [&](literal v){return l==v;});
    });


    auto pure = view::concat(pos_only,

                             neg_only
                             | view::transform([](literal l){return -l;;})
                             );

    auto rngf = _f | view::filter([&pure](auto s){
            return !intersection2(s, pure);
        })
            | to_vector;

#ifdef DEBUG
    std::cout << "Pure literals:"; std::cout << pure; std::cout << '\n';
    std::cout << "Formula before:"; print(std::cout); std::cout << '\n';
#endif

    if(pure){
         _f.clear();
        _f = rngf;

#ifdef DEBUG
    std::cout << "Formula after:"; print(std::cout); std::cout << '\n';
#endif
        return true;
    }else{
        return false;
#ifdef DEBUG
    std::cout << "Formula after:"; print(std::cout); std::cout << '\n';
    std::cout << "End:Pure literal\n";
#endif
}

    };


bool Formula::_unitPropagate2()
{
    auto units = _f | view::transform([](auto s){
            if (s.size()==1)
                return *(s.cbegin());//ovo probati bolje
            else
                return 0;
        })
            | view::filter([](auto value){return value != 0;});

#ifdef DEBUG
    std::cout << "Begin:Unit propagate\nUnit literals:"; std::cout << units; std::cout << '\n';
    std::cout << "Formula before:"; print(std::cout);
#endif

    auto Fp = _f | view::filter([&units](auto clause){
            return !intersection2(clause, units);
        });

       //sad izbacujemo svaki element iz unitsa samo sa suprotnim znakom
       auto neg_units = units | view::transform([](literal unit){return -unit;});
       //std::cout << neg_units;

       //ovaj deo ovde ima problem
       auto Fpp = Fp
               | view::transform([&neg_units](std::set<literal> &s){

               for(literal l: neg_units){

               auto it = std::find(s.begin(), s.end(), l);
               if(it!=std::end(s)){
                 s.erase(it);
                }


                }
               if(s.size()==0)
                    std::cout << "Empty clause found!";
               return s;

               })
               | to_vector;

        if(units){
           std::cout << "Formula after unit propagation:\n";
           log(Fpp);//vektor vektor int

            _f.clear(); //da li ovo zadrzava prazne skupove
            _f = Fpp;
#ifdef DEBUG
    std::cout << "Formula after:"; print(std::cout);
    std::cout << "End unit propagation\n";
#endif


           return true;
            }
       else {
#ifdef DEBUG
    std::cout << "Formula after:"; print(std::cout);
    std::cout << "End unit propagation\n";
#endif
              return false;

}


};
// Resolution
// Returs true if clause after resolution is tautology


clause& Formula::_resolution2(clause &first, clause &second, literal p)
{


    clause *cres = new clause;
    auto resolvent = view::concat(first, second)
            | view::filter([&p](literal i){

            if(i==p || i==(-p))
            return false;
            return true;});


    ranges::for_each(resolvent, [cres](literal l){cres->insert(l);/*std::cout << l << '.';*/});
//    std::cout << '\n';
    return *cres;
};



//vraca false ako je dodata prazna klauza, true ako je dodata ok klauza
bool Formula::_eliminate2(literal p)
{
#ifdef DEBUG
std::cout << "Begin: Elimination by variable:" << p << '\n';
std::cout << "Formula before"; print(std::cout);
#endif

auto first = std::find_if(std::begin(_f), std::end(_f),[&p](clause c){return std::find(c.begin(), c.end(), p)!=std::end(c);});
auto second = std::find_if(std::begin(_f), std::end(_f),[&p](clause c){return std::find(c.begin(), c.end(), -p)!=std::end(c);});


if(first!=std::end(_f) && second!=std::end(_f)){
    clause *newclause = new clause;

    *newclause = _resolution2(*first, *second, p);
    if(newclause->size()==0) {std::cout << "Empty clause found in resolution."; return true;}
    if(!_checkClause(*newclause)){
            _f.push_back(*newclause);
        }


    if(first<second)
        {
            _f.erase(second);
            _f.erase(first);
        }else{
            _f.erase(first);
            _f.erase(second);
        }

    }
#ifdef DEBUG
std::cout << "Formula after:"; print(std::cout);
std::cout << "End: Elimination by variable:" << p << '\n';

#endif
return false;
}

bool Formula::_getLiterals()
{
    unsigned previous = _literals.size();
    auto pos2 = _f | view::join;
    _literals.clear();
    for(auto val: pos2)
        _literals.insert(val);

    return previous != _literals.size();
}

//Checks if clause exists or is always true
bool Formula::_checkClause(clause c){
    auto eq = [](auto val1, auto val2){return val1==val2;};
    auto rng = c | view::all | view::transform([](literal l){return std::abs(l);})
             | to_vector
        | action::sort;


    auto rng2 = rng | view::group_by(eq);

    bool exists = ranges::any_of(_f, [&c](clause i){return i==c;});
    if(exists) {std::cout << "Clause "; wcls(c); std::cout << "already exists\n";return true;}

    return ranges::any_of(rng2, [](auto r){return (r | to_vector).size() > 1;});
}

