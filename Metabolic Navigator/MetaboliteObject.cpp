#include "MetaboliteObject.h"

using namespace std;

MetaboliteObject::MetaboliteObject(void)
{
}

MetaboliteObject::MetaboliteObject(Compound *compound, string compartment)
{
    MetaboliteObject::compound = compound;
    MetaboliteObject::compartment = compartment;
}

MetaboliteObject::MetaboliteObject(string compoundWithCompartment)
{
}


MetaboliteObject::~MetaboliteObject(void)
{
}

bool MetaboliteObject::isLabeled(void)
{
    for(unsigned int n=0;n<label.size();n++)
        if(label[n]!=0)
            return true;
    return false;
}

string MetaboliteObject::labelText(void)
{
    if(label.size() == 0)
        return "";

    string labelText;
    bool first = true;
    for(unsigned int n=0;n<label.size();n++)
        if(label[n] != 0)
        {
            if(first)
            {
                labelText += string("C") + string(QString().setNum(n+1).toAscii());
                first = false;
            }
            else
                labelText += "_" + string("C") + string(QString().setNum(n+1).toAscii());
        }
    return labelText;
}

string MetaboliteObject::key(void)
{
    if(label.size() != 0)
    {
        /*
        string labelText;
        bool first = true;
        for(unsigned int n=0;n<label.size();n++)
            if(label[n] != 0)
                if(first)
                {
                    labelText += string("C") + string(QString().setNum(n+1).toAscii());
                    first = false;
                }
                else
                    labelText += "_" + string("C") + string(QString().setNum(n+1).toAscii());
                    */
        if(!compartment.empty())
            return string(compound->cNumber+"_"+compartment+"_"+labelText());
        else
            return string(compound->cNumber+"_"+labelText());
    }
    else
    {
        if(!compartment.empty())
            return string(compound->cNumber+"_"+compartment);
        else
            return string(compound->cNumber);
    }
}

string MetaboliteObject::text(Pool *pool, bool withNames, bool withCompartments)
{
    string temp;
    temp = compound->getName();
    if(withCompartments &&
        compartment.length() != 0 &&
        compartment != "none")
        temp += "_" + compartment;
    return temp;
}

