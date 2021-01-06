#pragma once
#include <string>
#include <vector>


struct Atom
{
    std::string symbol;
    std::string atomLetter;
    double x;
    double y;
};

typedef std::vector<Atom>::iterator T_atomsIterator;


class CompoundDefinition
{
public:
    CompoundDefinition(void);
    ~CompoundDefinition(void);

    void parseFromKeggRPair(std::string text);
    unsigned int length(void);
    std::vector<Atom> atoms;
    std::string compoundID;
};
