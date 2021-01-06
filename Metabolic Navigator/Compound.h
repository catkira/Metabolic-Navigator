#pragma once
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include "boost/shared_ptr.hpp"
#include "ChemFormula.h"

class Reaction;
class ChemFormula;
class CompoundDefinition;

class Compound
{
public:
    Compound(void);
    ~Compound(void);
    void clear(void);
    Compound(const Compound &C);
    std::string getName(void);

    std::string cNumber;
    std::map<unsigned int, std::string> trivialNames;
    std::vector<std::string> names;
    std::map<unsigned int, unsigned int> isomeres;
    std::vector<std::pair<Reaction *, unsigned int>> linkedReactions;
    ChemFormula formula;
    std::string inchi;
    std::set<std::string> sameAs;
    std::vector<std::vector<int>> homotopes;
    std::vector<std::vector<int>> prochirales;
    CompoundDefinition *definition;
private:
};
