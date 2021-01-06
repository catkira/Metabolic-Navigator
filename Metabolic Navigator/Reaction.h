#pragma once
#include <string>
#include <map>
#include "boost/shared_ptr.hpp"
#include <vector>
#include <set>

class Equation;
class MyAlgorithms;
class CompoundTransition;
class Reaction;

const unsigned char RS_DIRECTION_BOTH = 0;
const unsigned char RS_DIRECTION_FORWARD = 1;
const unsigned char RS_DIRECTION_BACKWARD = 2;
const unsigned char RS_DIRECTION_NONE = 3;



bool compareReactions(const Reaction &r1, const Reaction &r2, bool &isReverse);
bool compareReactions2(const Reaction &r1, const Reaction &r2, bool &isReverse);


class Reaction
{
public:
    Reaction(void);
    ~Reaction(void);
    Reaction(const Reaction &R);
    void clear(void);
    boost::shared_ptr<Equation> equation;
    std::string id;
    std::string source;
    //set<string> globalCompartments;
    std::vector<std::string> names;
    //std::map<std::pair<std::string,std::string>,std::string> rpair;
    std::vector<CompoundTransition*> linkedCompoundTransitions;
    unsigned int dataset;
    bool isTransporter;

    unsigned char userSettings;
};


