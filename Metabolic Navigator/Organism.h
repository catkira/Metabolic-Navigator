#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>

class Reaction;
class OrganismBuildRule;

class Organism
{
public:
    Organism(void);
    ~Organism(void);
    void setName(std::string name){name_=name;};
    std::string name(void){return name_;};
    std::set<Reaction*> reactions;
    std::vector<std::string> reactionIDs;

    std::vector<std::string> compartments;
    std::map<std::string,std::string> outsideCompartment;

private:
    std::string name_;
    OrganismBuildRule* rule_;
};
