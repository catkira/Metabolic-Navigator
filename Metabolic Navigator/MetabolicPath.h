#pragma once
#include <list>
#include <vector>
#include <string>
#include <set>


const bool direction_top_down=true;
const bool direction_bottom_up=false;

class Pool;
class Reaction;
class MetaboliteObject;
class Compound;

template<typename T_reaction, typename T_metabolite>
class MetabolicPath
{
public:
    void append(const std::vector<T_reaction *> reactions, T_metabolite *metabolite);
    void setStartMetabolite(T_metabolite *metabolite);
    void setEndMetabolite(T_metabolite *metabolite);
    void clear(void);
    void generateTextOutput(std::string *textOut, Pool *pool=0);
    unsigned int length(void);
    bool contains(T_metabolite *);
    bool direction;

    std::vector<std::vector<T_reaction *>> reactions;
    std::vector<T_metabolite *> metabolites;
    std::set<T_metabolite *> metabolitesSet;
private:
};