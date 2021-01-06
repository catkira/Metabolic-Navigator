#include "MetabolicPath.h"
#include "Reaction.h"
#include "MetaboliteObject.h"
#include "Compound.h"
#include "equation.h"

using namespace std;

template class MetabolicPath<Reaction, MetaboliteObject>;

template<typename T_reaction, typename T_metabolite>
void MetabolicPath<T_reaction, T_metabolite>::
setStartMetabolite(T_metabolite *metabolite)
{
    metabolites.push_back(metabolite);
    metabolitesSet.insert(metabolite);
    direction = direction_top_down;
}

template<typename T_reaction, typename T_metabolite>
void MetabolicPath<T_reaction, T_metabolite>::
setEndMetabolite(T_metabolite *metabolite)
{
    metabolites.push_back(metabolite);
    metabolitesSet.insert(metabolite);
    direction = direction_bottom_up;
}



template<typename T_reaction, typename T_metabolite>
void MetabolicPath<T_reaction, T_metabolite>::
append(const std::vector<T_reaction *> reactions, T_metabolite *metabolite)
{
    metabolites.push_back(metabolite);
    metabolitesSet.insert(metabolite);

    MetabolicPath::reactions.push_back(reactions);
}


template<typename T_reaction, typename T_metabolite>
void MetabolicPath<T_reaction, T_metabolite>::
clear(void)
{
    reactions.clear();
    metabolites.clear();
    metabolitesSet.clear();
}

template<>
void MetabolicPath<Reaction, MetaboliteObject>::
generateTextOutput(string *textOut, Pool *pool)
{

    textOut->clear();
    if(metabolites.empty() == true)
        return;

    if(direction == direction_bottom_up)
    {
        unsigned int reactionsSize = reactions.size();
        for(unsigned int i=reactionsSize;i>0;i--)
        {
            textOut->append(metabolites[i]->compound->names.begin()->c_str() + string(" (") + 
                metabolites[i]->compound->cNumber + string(")\n"));
            if(pool!=0)
                textOut->append(" -> " + 
                    reactions.at(i-1).front()->id + ": (" + reactions.at(i-1).front()->equation->text(true,true,pool) + 
                    string(") ->\n"));
            else
                textOut->append(" -> " + 
                    reactions.at(i-1).front()->id + ": (" + reactions.at(i-1).front()->equation->text() + 
                    string(") ->\n"));
        }
        textOut->append(metabolites[0]->compound->names.begin()->c_str() +  
            string(" (") + metabolites[0]->compound->cNumber + string(")\n"));
    }
    else
    {
        textOut->append(metabolites[0]->compound->names.begin()->c_str() +  
            string(" (") + metabolites[0]->compound->cNumber + string(")\n"));
        for(unsigned int i=0;i<reactions.size();i++)
        {
            textOut->append(" -> " + 
                reactions.at(i-1).front()->id + ": (" + reactions.at(i-1).front()->equation->text() + 
                string(") ->\n"));
            textOut->append(metabolites[i+1]->compound->names.begin()->c_str() + string(" (") + 
                metabolites[i+1]->compound->cNumber + string(")\n"));
        }
    }

}

template<typename T_reaction, typename T_metabolite>
unsigned int MetabolicPath<T_reaction, T_metabolite>::
length(void)
{
    return reactions.size();
}

template<typename T_reaction, typename T_metabolite>
bool MetabolicPath<T_reaction, T_metabolite>::
contains(T_metabolite *metabolite)
{
    if(metabolitesSet.find(metabolite) == metabolitesSet.end())
        return false;
    else
        return true;
}