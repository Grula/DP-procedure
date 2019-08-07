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



bool intersection2(auto rng1,auto rng2){
    bool exists = false;
    //std::vector<literal> r1 = rng1 | to_vector;
    //std::vector<literal> r2 = rng2 | to_vector;

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

    out << '}' << '\n';
        }
    out << '}';
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
         //if(hasEmptyClause())
         //    return false;

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



// Unit Propagate
bool Formula::_unitPropagate()
{
	bool found_unit = false;

	for(auto start = _f.begin(); start != _f.end(); start++)
	{
		if((*start).size() == 1)
		{
			// Get single literal
			literal l = *(*start).begin();
			// Remove other clauses that contain given literal
			_f.erase(
				std::remove_if(_f.begin(),_f.end(),
						  	  [=] (clause c) {
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
	// std::cout << "Unit prop " << found_unit << std::endl;

        return found_unit;
}

bool Formula::_pureLiteral2()
{
    //uzimamo samo pozitivne literale
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
    std::cout << pure << pos_only;
    //std::cout << intersection2(pos_only, pure);
    std::cout << "\n\n\n";
    log(_f);


    auto rngf = _f | view::filter([&pure](auto s){
            return !intersection2(s, pure);
        })
            | to_vector;

    if(pure){
        std::cout << "After pure literal!" << std::endl;
        _f.clear();
        _f = rngf;
        log(_f);

        return true;
    }else
        return false;

};

// Pure Literal
bool Formula::_pureLiteral()
{
	bool found_pure = false;
	_literalsCount.clear(); // try to fill this just once
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
	// for(clause c : _f)
	// {
	// 	for(literal l : c)
	// 	{
	// 		std::cout << l << " ";
	// 	}
	// 	std::cout << std::endl;
	// }
	// std::cout << "edning f" << std::endl;
	for(auto start = _literalsCount.begin(); start != _literalsCount.end(); start++)
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
	// std::cout << "Pure lit " << found_pure << std::endl;

	return found_pure;	

}

bool Formula::_unitPropagate2()
{
    auto units = _f | view::transform([](auto s){
            if (s.size()==1)
                return *(s.cbegin());//ovo probati bolje
            else
                return 0;
        })
            | view::filter([](auto value){return value != 0;});

    std::cout << units;
    /*std::set<literal> s1 = {1,2,3};
    std::set<literal> s2 = {1,-2,3};
    std::set<literal> s3 = _resolution2(s1,s2,2);
   for(auto e: s3)
       std::cout << e << '^';

   std::cout << '\n';
*/
    //std::cout << "eliminate" << _eliminate2(-3);
    log(_f);
    auto Fp = _f | view::filter([&units](auto clause){
            return !intersection2(clause, units);
        });

    /*auto intersection = [](auto rng1, auto rng2){
            bool exists;
            ranges::for_each(rng1, [&exists, &rng2](literal l){
                exists = ranges::count(rng2, l)>0;
               // std::cout << !exists << 'E';
                });
            return exists;

        };
*/

        //log(Fp);

       //sad izbacujemo svaki element iz unitsa samo sa suprotnim znakom
       auto neg_units = units | view::transform([](literal unit){return -unit;});
       //std::cout << neg_units;

       //ovaj deo ovde ima problem
       auto Fpp = Fp
               | view::transform([&neg_units](std::set<literal> &s){

               for(literal l: neg_units){

               auto it = std::find(s.begin(), s.end(), l);
               if(it!=std::end(s)){
                //std::cout << "before";wcls(s);
                 s.erase(it);
                //std::cout << "after";wcls(s);
                }


                }
               if(s.size()==0) std::cout << "\nPrazna je\n";
               return s;

/*
               auto ts = s
               | view::transform([&neg_units](literal l){
               if(ranges::count(neg_units, l) > 0)
                    return 0;
               else
                    return l;
               })
               | view::filter([](literal l){return l!=0;})
               ;
               return ts | to_vector;*/
               })
               | to_vector;


        if(units){
           std::cout << "Formula after unit propagation:\n";
           log(Fpp);//vektor vektor int

            _f.clear(); //da li ovo zadrzava prazne skupove
            _f = Fpp;
        //da se kastuje vektor vektor intova u vektor skup intova
       /* for(std::vector<int> v : Fpp)
            {   std::set<int> s(v.begin, v.end());

                _f.emplace(s);
            }
        */
           return true;
            }
       else
            return false;
       //   std::cout << 'R' << ranges::size(Fpp)<< 'R';
        //kastujem u vektor skupova

};
// Resolution
// Returs true if clause after resolution is tautology

bool Formula::_resolution(clause &first, clause &second, literal p)
{
	// Erase p from frist clause and ~p from second clause
	first.erase(first.find(p));
	second.erase(second.find(-p));

	first.merge(second);
	// Search if exists q and ~q
	for(literal l : first)
	{
		if(first.find(-l) != first.end())
			return true;
	}
        return false;
};

clause& Formula::_resolution2(clause &first, clause &second, literal p)
{


    //mozda ne treba ova provera
    //auto it = ranges::find(c1,p);
    //auto it2 = ranges::find(c2, -p);
    //sta se desava ako klauze ne sadrze p i -p
    clause *cres = new clause;
    auto resolvent = view::concat(first, second)
            | view::filter([&p](literal i){

            if(i==p || i==(-p))
            return false;
            return true;});


    ranges::for_each(resolvent, [cres](literal l){cres->insert(l);/*std::cout << l << '.';*/});
    //std::vector<literal> v = resolvent | to_vector;
    //ranges::for_each(cres, [](literal l){std::cout << l << '.';});
    std::cout << '\n';
    return *cres;
};


// Eliminate variable
bool Formula::_eliminate(literal l)
{
	for (size_t i = 0; i < _f.size(); i++)
	{
		for(size_t j = i + 1; j < _f.size(); j++)
		{
			// Check if our clauses contains given literals
			if(_f[i].find(l) != _f[i].end() && _f[j].find(-l) != _f[j].end())
			{

				// Check if given answer is not tautology
				if(!_resolution(_f[i], _f[j], l))
				{
					_f.erase(_f.begin() + (j - 1));
					// Check if clause is empty
					if(_f[i].size() == 0)
					{
						// Erase first caluse and return false
						_f.erase(_f.begin() + i);
						return false;
					}
				}
				// Clause is tautology so delete first and second clause
				else 
				{
					_f.erase(_f.begin() + i);
					_f.erase(_f.begin() + (j - 1));
				}
				// for(literal l : _f[i])
				// {
				// 	std::cout << l << " ";
				// }				
			}
		}
	}
        return true;
}

//vraca false ako je dodata prazna klauza, true ako je dodata ok klauza
bool Formula::_eliminate2(literal p)
{

auto first = std::find_if(std::begin(_f), std::end(_f),[&p](clause c){return std::find(c.begin(), c.end(), p)!=std::end(c);});
auto second = std::find_if(std::begin(_f), std::end(_f),[&p](clause c){return std::find(c.begin(), c.end(), -p)!=std::end(c);});

std::cout << "eliminate" << p << '\n';
log(_f);
if(first!=std::end(_f) && second!=std::end(_f)){
    clause *newclause = new clause;

    *newclause = _resolution2(*first, *second, p);
    if(newclause->size()==0) {std::cout << "Prazna je!"; return true;}
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

log(_f);
return false;
}

//vraca true ako je nastalo izmene
bool Formula::_getLiterals()
{
    unsigned previous = _literals.size();
    auto pos2 = _f | view::join;// | to_vector | action::sort | action::unique;
    _literals.clear();
    for(auto val: pos2)
        _literals.insert(val);

    return previous != _literals.size();
}


//treba uraditi
void Formula::_processClauses()
{
    //eliminise sve klauze koje imaju suprotne literale

    _f.erase(std::remove_if(_f.begin(), _f.end(), [](auto c){

         //dato je c vratiti da li klauza sadrzi l i -l
        return true;
        }),std::end(_f));
};



bool Formula::hasEmptyClause(){
    return ranges::any_of(_f, [](clause c){return c.size()==0;});
}

//O(n log n) u prosecnom slucaju zbog sorta
bool Formula::_checkClause(clause c){
    auto eq = [](auto val1, auto val2){return val1==val2;};


auto rng = c | view::all | view::transform([](literal l){return std::abs(l);})
             | to_vector
        | action::sort;
auto rng2 = rng | view::group_by(eq);
//std::cout << rng2;
return ranges::any_of(rng2, [](auto r){return (r | to_vector).size() > 1;});

}


