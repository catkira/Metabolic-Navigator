#include "Equation.h"
#include "Helper.h"

using namespace std;

unsigned int getMetaboliteDirection(unsigned int arg)
{
    return (arg & 0x110000);
}


Equation::Equation(void)
{
    valid=false;
    stoichiometryType = withStoichiometry;
}

Equation::Equation(MyAlgorithms *ma, string equation, set<string> *availableCompartments, map<string, MetaboliteObject*> *metabolitesPool)
{
    parseEquation(ma, equation,availableCompartments, metabolitesPool);
}

Equation::Equation(MyAlgorithms *ma, string equation,  bool *valid, set<string> *availableCompartments, map<string, MetaboliteObject*> *metabolitesPool)
{
    *valid=parseEquation(ma, equation, availableCompartments, metabolitesPool);
}

Equation::Equation(MyAlgorithms *ma, const std::multimap<Compound*, unsigned int>& equation, 
    std::map<std::string, MetaboliteObject*> *metabolitesPool)
{
    parseEquation(ma, equation, metabolitesPool);
}

void Equation::parseEquation(MyAlgorithms *ma, const std::multimap<Compound*, unsigned int>& equation, std::map<std::string, MetaboliteObject*> *metabolitesPool)
{
    stoichiometryType = withoutStoichiometry;
    compartmentType = withoutCompartments;
    directionType = direction_both;

    MetaboliteObject metaboliteObject;
    std::multimap<Compound*, unsigned int>::const_iterator equationIterator = equation.begin();

    while(equationIterator != equation.end())
    {
        //pMetaboliteObject = ma->getOrCreateMetaboliteObject(equationIterator->first,"");
        metaboliteObject.compartment = "";
        metaboliteObject.compound = equationIterator->first;
        metabolites.push_back(pair<MetaboliteObject, unsigned int>(metaboliteObject, equationIterator->second | 1));

        ++equationIterator;
    }
    valid = true;
}


/*
Equation::Equation(const Equation &E)
{
    *this = E;
}
*/

Equation::~Equation(void)
{
}

void Equation::setStiochiometryType(const StoichiometryType& type)
{
    stoichiometryType = type;
}

void Equation::setCompartmentType(const CompartmentType& type)
{
    compartmentType = type;
}

void Equation::clear(void)
{
    metabolites.clear();
    valid = false;
}

string Equation::getLastError(void)
{
    return errorMessage;
}

/*
void Equation::createOrLinkMetaboliteObjects(map<string, MetaboliteObject> *metabolitesPool, Compound *compounds)
{
}

void Equation::calculateReachableMetabolites(
    MetaboliteObject *startMetabolite, 
    set<MetaboliteObject*> reachableMetabolites,
    bool withCompartments)
{
    
}
*/

unsigned int Equation::position(const T_equationMetabolite& equationMetabolite)
{
    return equationMetabolite.second & 0x110000;
}

bool Equation::parseEquation(MyAlgorithms *ma, string equation, set<string> *availableCompartments, map<string, MetaboliteObject*> *metabolitesPool)
{
    int val,val2,val3;
    if(equation.find_first_of("<") != string::npos && equation.find_first_of(">")!=string::npos)
    {
        directionType = direction_both;
    }
    else if(equation.find_first_of("<") != string::npos)
    {
        directionType = direction_left;
    }
    else if(equation.find_first_of(">") != string::npos)
    {
        directionType = direction_right;
    }
    else
    {
        directionType = direction_both;
    }
    
    bool go=true;
    unsigned int faktor;
    string cNumber;
    bool left=true;
    
    if(equation.length()<10)
    {
        errorMessage = "equation "+ equation + "is too short";
        return false;
    }

    val = 0;
    bool stoich;
    bool comp=false;
    //Compounds.clear();
    metabolites.clear();
    if(equation[0] == ' ')
        val = equation.find_first_not_of(" ",0);
    faktor=1;
    unsigned int numCompounds=0;
    string tempCompartment;
    while(go==true)
    {
        val2 = equation.find_first_of(" ",val);     
        if(val2 == string::npos)
        {
            if(val > (equation.size()-3))
            {
                go=false;
                continue;
            }
            val2 = equation.size();
        }
        stoich=false;
        if((val2-val)<5 && (equation[val]>='0' && equation[val]<='9'))    // stoichometry number
        {        
            if(equation.substr(val,val2-val).c_str()==string("n"))
                faktor=STOICH_N;
            else
                faktor=strtol(equation.substr(val,val2-val).c_str(),0,10);
            val=val2;
            stoich=true;
        }
        else
        {
            cNumber=equation.substr(val,val2-val);
            val3=equation.find_first_of('_',val);
            if(val3>val2)
                val3=string::npos;
            if((val3!=string::npos && numCompounds!=0 && comp==false) || (val3==string::npos && comp==true && cNumber!=string("Biomass")))
            {
                errorMessage = "incomplete compartment information in equation " + equation;
                valid = false;
                return false;
            }
            if(val3!=string::npos)
            {
                cNumber=equation.substr(val,val3-val);
                //compartments[cNumber]=equation.substr(val3+1,val2-(val3+1));
                tempCompartment = equation.substr(val3+1,val2-(val3+1));;
                availableCompartments->insert(tempCompartment);
                comp=true;
            }
            numCompounds++;
        }
        
        if(!stoich)
        {
            MetaboliteObject metaboliteObject;
            Compound *pCompound;
            if(left)
            {
                //Compounds.push_back(pair<string, unsigned int>(cNumber, METABOLITE_IS_LEFT | faktor)); 
                
                pCompound = ma->getOrCreateCompound(cNumber);
                pCompound->cNumber = cNumber;
                //metaboliteObject = ma->getOrCreateMetaboliteObject(pCompound, tempCompartment);
                metaboliteObject.compartment = tempCompartment;
                metaboliteObject.compound = pCompound;
                metabolites.push_back(pair<MetaboliteObject, unsigned int>(metaboliteObject, METABOLITE_IS_LEFT| faktor));

                //temp = getOrCreateMetaboliteObject(compound, tempCompartment, metabolitesPool);
                //metabolites.push_back(pair<MetaboliteObject*, unsigned int>(temp, METABOLITE_IS_LEFT | faktor));
                faktor=1;
            }
            else
            {
                //Compounds.push_back(pair<string, unsigned int>(cNumber, METABOLITE_IS_RIGHT| faktor)); 

                pCompound = ma->getOrCreateCompound(cNumber);
                pCompound->cNumber = cNumber;
                //pMetaboliteObject = ma->getOrCreateMetaboliteObject(pCompound, tempCompartment);
                metaboliteObject.compartment = tempCompartment;
                metaboliteObject.compound = pCompound;
                metabolites.push_back(pair<MetaboliteObject, unsigned int>(metaboliteObject, METABOLITE_IS_RIGHT| faktor));

                faktor=1;
            }
        }
        
        stoich = !stoich;

        val = equation.find_first_not_of(" ",val2);  
        if(val==string::npos)
        {
            go=false;
            continue;
        }
        if(equation[val]=='+')
        {
            val = equation.find_first_not_of(" ",val+1); 
            continue;
        }
        if(equation[val]=='<' || equation[val]=='=' || equation[val]=='>' || equation[val]=='-')
        {
            left=false;
            val=equation.find_first_of(" ",val); 
            val=equation.find_first_not_of(" ",val); 
            if(val==string::npos)
                go=false;
        }
    }
    rawEquation = equation;
    valid = true;
    return true;
}

bool Equation::isValid(void)
{
    return valid;
}

bool Equation::containsMetabolite(const T_equationMetabolite &searchMetabolite)
{
    T_equationMetabolitesIterator equationMetabolitesIterator;

    equationMetabolitesIterator = metabolites.begin();
    while(equationMetabolitesIterator != metabolites.end())
    {
        if(equationMetabolitesIterator->first.compound == searchMetabolite.first.compound)
        {
            if(searchMetabolite.first.compartment=="")
                return true;
            if(equationMetabolitesIterator->first.compartment == searchMetabolite.first.compartment)
                return true;
            if(globalCompartments.find(searchMetabolite.first.compartment) 
                != globalCompartments.end())
                return true;
        }
        ++equationMetabolitesIterator;
    }
    return false;
}


string Equation::text(bool withCompartments, bool withNames, Pool* pool)
{
    string ret="";
    if(valid==false)
        return ret;
    char buf[10];
    //map<string, unsigned int>::iterator equationCompoundsIterator;
    T_equationMetabolitesIterator equationMetabolitesIterator;
    unsigned int stoich;
    string leftPart, middlePart, rightPart;
    equationMetabolitesIterator = metabolites.begin();
    for(unsigned int n=0;n<metabolites.size();n++)
    {
        stoich = (equationMetabolitesIterator->second & 0xFFFF);
            _itoa_s(stoich,&buf[0],10,10);
            if((equationMetabolitesIterator->second & METABOLITE_IS_LEFT)!=0)
            {
                if(stoich!=1)
                {
                    leftPart.append(buf);
                    leftPart.append(" ");
                }
                if(withNames)
                    //leftPart.append(getCompoundName(equationMetabolitesIterator->first->compound->cNumber,pool));
                    leftPart.append(equationMetabolitesIterator->first.compound->getName());
                else
                    leftPart.append(equationMetabolitesIterator->first.compound->cNumber);
                if(withCompartments)
                {
                    if(equationMetabolitesIterator->first.compartment.length() != 0 &&
                        equationMetabolitesIterator->first.compartment != "")
                    {
                        leftPart.append("_");
                        leftPart.append(equationMetabolitesIterator->first.compartment);
                    }
                }
                leftPart.append(" + ");
            }
            else
            {
                if(stoich!=1)
                {
                    rightPart.append(buf);
                    rightPart.append(" ");
                }
                if(withNames)
                    rightPart.append(equationMetabolitesIterator->first.compound->getName());
                else
                    rightPart.append(equationMetabolitesIterator->first.compound->cNumber);
                if(withCompartments)
                {
                    if(equationMetabolitesIterator->first.compartment.length() != 0 &&
                        equationMetabolitesIterator->first.compartment != "none")
                    {
                        rightPart.append("_");
                        rightPart.append(equationMetabolitesIterator->first.compartment);
                    }
                }
                rightPart.append(" + ");
            }
        equationMetabolitesIterator++;
    }
    if(directionType ==  direction_both)
        middlePart.append(" <=> ");
    else if(directionType == direction_left)
        middlePart.append(" <= ");
    else
        middlePart.append(" => ");
    if(leftPart.size()>1)
        leftPart.resize(leftPart.size()-2);
    if(rightPart.size()>1)
        rightPart.resize(rightPart.size()-2);
    return leftPart+middlePart+rightPart;
}

/*
string Equation::text(bool withCompartments)
{
    string ret="";
    if(valid==false)
        return ret;
    char buf[10];
    //map<string, unsigned int>::iterator equationCompoundsIterator;
    vector<pair<string, unsigned int>>::iterator equationCompoundsIterator;
    unsigned int stoich;
    string leftPart, middlePart, rightPart;
    equationCompoundsIterator = Compounds.begin();
    for(unsigned int n=0;n<Compounds.size();n++)
    {
        stoich = (equationCompoundsIterator->second & 0xFFFF);
            _itoa_s(stoich,&buf[0],10,10);
            if((equationCompoundsIterator->second & METABOLITE_IS_LEFT)!=0)
            {
                if(stoich!=1)
                {
                    leftPart.append(buf);
                    leftPart.append(" ");
                }
                leftPart.append(equationCompoundsIterator->first);
                if(withCompartments)
                {
                    if(compartments.find(equationCompoundsIterator->first) != compartments.end())
                    {
                        leftPart.append("_");
                        leftPart.append(compartments[equationCompoundsIterator->first]);
                    }
                }
                leftPart.append(" + ");
            }
            else
            {
                if(stoich!=1)
                {
                    rightPart.append(buf);
                    rightPart.append(" ");
                }
                rightPart.append(equationCompoundsIterator->first);
                if(withCompartments)
                {
                    if(compartments.find(equationCompoundsIterator->first) != compartments.end())
                    {
                        rightPart.append("_");
                        rightPart.append(compartments[equationCompoundsIterator->first]);
                    }
                }
                rightPart.append(" + ");
            }
        equationCompoundsIterator++;
    }
    if(directionType ==  direction_both)
        middlePart.append(" <=> ");
    else if(directionType == direction_left)
        middlePart.append(" <= ");
    else
        middlePart.append(" => ");
    if(leftPart.size()>1)
        leftPart.resize(leftPart.size()-2);
    if(rightPart.size()>1)
        rightPart.resize(rightPart.size()-2);
    return leftPart+middlePart+rightPart;
}
*/

/*

string Equation::getCompoundName(string cNumber, Pool *pool)
{
    T_compoundsIterator compoundsIterator = pool->compounds->begin();
    while(compoundsIterator != pool->compounds->end())
    {
        if( compoundsIterator->second.cNumber == cNumber &&
            compoundsIterator->second.names.size()!= 0)
        {
            if(compoundsIterator->second.names[0].length() != 0)
                return compoundsIterator->second.names[0];
        }
        compoundsIterator++;
    }
    return cNumber;
}
*/

/*
string Equation::textWithNames(Pool *pool, bool withCompartments)
{
    string ret="";
    if(valid==false)
        return ret;
    char buf[10];
    //map<string, unsigned int>::iterator equationCompoundsIterator;
    vector<pair<MetaboliteObject*, unsigned int>>::iterator equationMetabolitesIterator;
    unsigned int stoich;
    string leftPart, middlePart, rightPart;
    equationMetabolitesIterator = metabolites.begin();
    for(unsigned int n=0;n<metabolites.size();n++)
    {
        stoich = (equationMetabolitesIterator->second & 0xFFFF);
            _itoa_s(stoich,&buf[0],10,10);
            if((equationMetabolitesIterator->second & METABOLITE_IS_LEFT)!=0)
            {
                if(stoich!=1)
                {
                    leftPart.append(buf);
                    leftPart.append(" ");
                }
                leftPart.append(getCompoundName(equationMetabolitesIterator->first->compound->cNumber,pool));
                if(withCompartments)
                {
                    if(equationMetabolitesIterator->first->compartment.length() != 0 &&
                        equationMetabolitesIterator->first->compartment != "none")
                    {
                        leftPart.append("_");
                        leftPart.append(equationMetabolitesIterator->first->compartment);
                    }
                }
                leftPart.append(" + ");
            }
            else
            {
                if(stoich!=1)
                {
                    rightPart.append(buf);
                    rightPart.append(" ");
                }
                rightPart.append(getCompoundName(equationMetabolitesIterator->first->compound->cNumber,pool));
                if(withCompartments)
                {
                    if(equationMetabolitesIterator->first->compartment.length() != 0 &&
                        equationMetabolitesIterator->first->compartment != "none")
                    {
                        rightPart.append("_");
                        rightPart.append(equationMetabolitesIterator->first->compartment);
                    }
                }
                rightPart.append(" + ");
            }
        equationMetabolitesIterator++;
    }
    if(directionType ==  direction_both)
        middlePart.append(" <=> ");
    else if(directionType == direction_left)
        middlePart.append(" <= ");
    else
        middlePart.append(" => ");
    if(leftPart.size()>1)
        leftPart.resize(leftPart.size()-2);
    if(rightPart.size()>1)
        rightPart.resize(rightPart.size()-2);
    return leftPart+middlePart+rightPart;
}
*/

/*
string Equation::textWithNames(Pool *pool, bool withCompartments)
{
    string ret="";
    if(valid==false)
        return ret;
    char buf[10];
    //map<string, unsigned int>::iterator equationCompoundsIterator;
    vector<pair<string, unsigned int>>::iterator equationCompoundsIterator;
    unsigned int stoich;
    string leftPart, middlePart, rightPart;
    equationCompoundsIterator = Compounds.begin();
    for(unsigned int n=0;n<Compounds.size();n++)
    {
        stoich = (equationCompoundsIterator->second & 0xFFFF);
            _itoa_s(stoich,&buf[0],10,10);
            if((equationCompoundsIterator->second & METABOLITE_IS_LEFT)!=0)
            {
                if(stoich!=1)
                {
                    leftPart.append(buf);
                    leftPart.append(" ");
                }
                leftPart.append(getCompoundName(equationCompoundsIterator->first,pool));
                leftPart.append(" + ");
            }
            else
            {
                if(stoich!=1)
                {
                    rightPart.append(buf);
                    rightPart.append(" ");
                }
                rightPart.append(getCompoundName(equationCompoundsIterator->first,pool));
                rightPart.append(" + ");
            }
        equationCompoundsIterator++;
    }
    if(directionType ==  direction_both)
        middlePart.append(" <=> ");
    else if(directionType == direction_left)
        middlePart.append(" <= ");
    else
        middlePart.append(" => ");
    if(leftPart.size()>1)
        leftPart.resize(leftPart.size()-2);
    if(rightPart.size()>1)
        rightPart.resize(rightPart.size()-2);
    return leftPart+middlePart+rightPart;
}
*/

bool Equation::hasMetaboliteCompartments(void)
{
    T_equationMetabolitesIterator equationMetabolitesIterator;
    equationMetabolitesIterator = metabolites.begin();
    while(equationMetabolitesIterator != metabolites.end())
    {
        if(equationMetabolitesIterator->first.compartment != "")
            return true;
        equationMetabolitesIterator++;
    }
    return false;
}