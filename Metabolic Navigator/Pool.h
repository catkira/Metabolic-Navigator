#pragma once
#include <map>
#include <string>
#include <set>
#include <vector>
#include "RPair.h"


class UserDataset;
class Compound;
class Equation;
class Reaction;
class Enzyme;
class MetaboliteObject;
class CompoundTransition;
class CompoundDefinition;
class Organism;
class RPair;
class FateMapCompound;

struct ltstr
{
    bool operator()(const std::string s1, const std::string s2) const
  {
    return s1 < s2;
  }
};


typedef std::map<std::string, Compound>::iterator T_compoundsIterator;
typedef std::map<std::string, Reaction>::iterator T_reactionsIterator;
typedef std::map<std::string, Reaction>::const_iterator T_reactionsConstIterator;
typedef std::set<std::string>::iterator T_selectedCompoundsIterator;
typedef std::map<std::string, Organism>::iterator T_organismsIterator;

typedef std::multimap<std::string, CompoundTransition, ltstr>::iterator T_compoundTransitionsIterator;
typedef std::multimap<std::string, RPair, ltstr>::iterator T_rpairsIterator;
typedef std::vector<Enzyme>::iterator T_enzymesIterator;


class Pool
{
public:
    Pool(void);
    ~Pool(void);

    std::string dataDir;
    std::string applicationDir;
    //std::string fateMapDir;

    std::map<unsigned int, UserDataset> *userDatasets;

    std::map<std::string, Compound> *compounds; 
    std::map<std::string, Compound*> *compoundNames;

    std::map<unsigned int, Equation> *equations;

    std::map<std::string, Reaction> *reactions;

    std::set<Reaction*> *selectedReactions2;
    std::set<std::string> *selectedCompounds;

    std::map<std::string, MetaboliteObject*> *metabolitesPool;

    std::set<std::string> *availableCompartments;

    std::multimap<std::string, CompoundTransition, ltstr> compoundTransitions;
    //std::multimap<std::string, RPair, ltstr> rpairs;

    std::vector<Enzyme> *enzymes;

    std::map<std::string, CompoundDefinition> compoundDefinitions;

    std::map<std::string, Organism> organisms;

    std::map<std::string, FateMapCompound> fateMapCompounds;

    std::map<std::string, Compound*> inchi2Compound;

    std::map<std::string, std::string> organismNames;
};
