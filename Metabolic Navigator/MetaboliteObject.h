#pragma once
#include <map>
#include "mylabel.h"
#include "Compound.h"
#include "boost/shared_ptr.hpp"
#include "Reaction.h"
#include "Pool.h"
#include "boost/numeric/ublas/vector.hpp"
#include "boost/numeric/ublas/io.hpp"

class MyLabel;
class Compound;
class Pool;

class MetaboliteObject
{
public:
    MetaboliteObject(void);
    MetaboliteObject(Compound *compound, std::string compartment);
    MetaboliteObject(std::string compoundWithCompartment);
    ~MetaboliteObject(void);
    std::string key(void);
    std::string text(Pool *pool, bool withNames=false, bool withCompartments=true);
    std::string labelText(void);
    bool operator==(const MetaboliteObject &lhs)
    {
        if(this->compound == lhs.compound && this->compartment == lhs.compartment)
            return true;
        return false;
    }
    bool isLabeled(void);
    Compound *compound;
    //MyLabel *imageObject;
    std::string displayText;
    //std::map<unsigned int, std::pair<unsigned int, MetaboliteObject *>> connectedTo;
    //unsigned int ID;
    std::string compartment;
    boost::numeric::ublas::vector<double> label;
private:
};
