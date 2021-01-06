#include "Compound.h"
#include "Helper.h"

#include "Reaction.h"
#include "CompoundDefinition.h"


using namespace std;

Compound::Compound(void)
{
    definition = 0;
}

Compound::~Compound(void)
{
}

Compound::Compound(const Compound &C)
{
    *this = C;
    this->definition = 0;
}

void Compound::clear(void)
{
    cNumber.clear();
    trivialNames.clear();
    names.clear();
    linkedReactions.clear();
    formula.clear();
    sameAs.clear();
    definition = 0;
}

string Compound::getName(void)
{
    if(names.size()!= 0)
    {
        if(names.begin()->length() != 0)
            return names.begin()->c_str();
    }
    return cNumber;
}

/*
bool Compound::matchesName(string pattern)
{
    int val;
    int nameSize= names.size();
    string pattern2;
    pattern2 = make_lowercase(pattern);
    val = pattern.find("*");
    if(val == string::npos)
    {
        for(unsigned int n=0;n<nameSize;n++)
        {
            if(make_lowercase(names[n])==pattern2)
                return true;
        }
    }
    else
    {
        if(val==(pattern.size()-1))
        {
            pattern2 = pattern2.substr(0,val);
            for(unsigned int n=0;n<nameSize;n++)
            {
                if(make_lowercase(names[n]).find(pattern2)==0)
                    return true;
            }
        }
        else if(val==0)
        {
            pattern2 = pattern2.substr(1,pattern2.size()-1);
            for(unsigned int n=0;n<nameSize;n++)
            {
                if(names[n].size() < pattern2.size())
                    continue;
                if(make_lowercase(names[n]).find(pattern2)==(names[n].size()-pattern2.size()))
                    return true;
            }
        }
    }
    return false;
}
*/