#include "Helper.h"

using namespace std;

bool splitCompound(string Compound, string *CompoundName, string *CompartmentName)
{
    return true;
}


std::string make_lowercase(std::string s) {
for_each(s.begin(), s.end(), lowercase_func());
return s;
}


/*
MetaboliteObject* getOrCreateMetaboliteObject(Compound *compound, 
    string compartment, 
    map<string,MetaboliteObject*> *metabolitesPool)
{
    map<string,MetaboliteObject*>::iterator metabolitesPoolIterator;
    MetaboliteObject *metaboliteObject;
    metabolitesPoolIterator = metabolitesPool->find(compound->cNumber+"_" +compartment);
    if(metabolitesPoolIterator == metabolitesPool->end())
    {
        metaboliteObject = new MetaboliteObject(compound,compartment);
        metabolitesPool->insert(pair<string,MetaboliteObject*>(metaboliteObject->key(),metaboliteObject));
    }
    else
        metaboliteObject = metabolitesPoolIterator->second;
    return metaboliteObject;
}
*/