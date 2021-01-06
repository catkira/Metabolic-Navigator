#pragma once
#include <string>
#include <list>
#include "Equation.h"
#include "boost/shared_ptr.hpp"

class Equation;

class UserDataset
{
public:
    UserDataset(void);
    ~UserDataset(void);
// almost the same as in Reaction
    boost::shared_ptr<Equation> equation;
    std::string scientist;
    std::string keggID;
    //set<string> globalCompartments;
// Dataset specific stuff
    std::list<std::string> ec;
    std::string comment;
    int confidence;
    int deltaG;
    bool accepted;
    bool excluded;
    bool isTransporter;
private:
};
