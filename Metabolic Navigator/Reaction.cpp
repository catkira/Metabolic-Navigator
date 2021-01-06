#include "Reaction.h"

#include "MyAlgorithms.h"
#include "Equation.h"


bool compareReactions2(const Reaction &r1, const Reaction &r2, bool &isReverse)
{
    Reaction tempR1 = r1;
    Reaction tempR2 = r2;

    T_equationMetaboliteIterator it;
    
    it = tempR1.equation->metabolites.begin();
    while(it != tempR1.equation->metabolites.end())
    {
        if(it->first.compound->cNumber == "C00080")
        {
            it = tempR1.equation->metabolites.erase(it);
            continue;
        }
        ++it;
    }
    if(tempR1.equation->metabolites.empty())
        return false;

    it = tempR2.equation->metabolites.begin();
    while(it != tempR2.equation->metabolites.end())
    {
        if(it->first.compound->cNumber == "C00080")
        {
            it = tempR2.equation->metabolites.erase(it);
            continue;
        }
        ++it;
    }
    if(tempR2.equation->metabolites.empty())
        return false;


    return compareReactions(tempR1, tempR2, isReverse);
}


bool compareReactions(const Reaction &r1, const Reaction &r2, bool &isReverse)
{
    T_equationMetaboliteIterator it1,it2;

    if(r1.equation->metabolites.size() != r2.equation->metabolites.size())
        return false;

    bool found = false;
    it1 = r1.equation->metabolites.begin();
    while(it1 != r1.equation->metabolites.end())
    {
        found = false;
        it2 = r2.equation->metabolites.begin();
        while(it2 != r2.equation->metabolites.end())
        {
            if(it2->first == it1->first && 
                getMetaboliteDirection(it2->second) == getMetaboliteDirection(it1->second))
            {
                found = true;
                break;
            }
            ++it2;
        }
        if(!found)
            break;
        ++it1;
    }

    isReverse=false;
    if(found)
        return true;

    found=false;
    it1 = r1.equation->metabolites.begin();
    while(it1 != r1.equation->metabolites.end())
    {
        found = false;
        it2 = r2.equation->metabolites.begin();
        while(it2 != r2.equation->metabolites.end())
        {
            if(it2->first == it1->first && 
                getMetaboliteDirection(it2->second) != getMetaboliteDirection(it1->second))
            {
                found = true;
                break;
            }
            ++it2;
        }
        if(!found)
            break;
        ++it1;
    }

    isReverse=true;
    if(found)
        return true;

    return false;
}



Reaction::Reaction(void)
{
    equation = (boost::shared_ptr<Equation>)new Equation();
    clear();
}

Reaction::~Reaction(void)
{
}


Reaction::Reaction(const Reaction &R)
{
    *this = R;
    this->equation = (boost::shared_ptr<Equation>)new Equation();
    *(this->equation) = *(R.equation);
}


void Reaction::clear(void)
{
    equation->clear();
    id.clear();
    source.clear();
    names.clear();
    //rpair.clear();
    linkedCompoundTransitions.clear();
    dataset = -1;
    isTransporter = false;
    userSettings = RS_DIRECTION_BOTH;
}

