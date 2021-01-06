#pragma once
#include <string>
#include <vector>

typedef std::vector<std::string>::iterator T_inOrganismsIterator;
typedef std::vector<std::string>::iterator T_inReactionsIterator;

class Enzyme
{
public:
    Enzyme(void);
    ~Enzyme(void);
    std::string ecNumber;
    std::vector<std::string> names;
    std::vector<std::string> inOrganisms;
    std::vector<std::string> inReactions;
};
