#pragma once
#include  <map>
#include <QString>
#include "Compound.h"
#include "MetaboliteObject.h"
#include <list>
#include "pool.h"
#include "MyAlgorithms.h"
#include "defines.h"

#define STOICH_N_MINUS_2 10008
#define STOICH_N_MINUS_1 10009
#define STOICH_N 10010
#define STOICH_N_PLUS_1 10011
#define STOICH_N_PLUS_2 10012

#define METABOLITE_IS_LEFT  0x010000
#define METABOLITE_IS_RIGHT 0x100000

unsigned int getMetaboliteDirection(unsigned int arg);

enum DirectionType
{
    direction_both,
    direction_right,
    direction_left,
    direction_dontCare
};

enum StoichiometryType
{
    withStoichiometry,
    withoutStoichiometry
};

enum CompartmentType
{
    withCompartments,
    withoutCompartments
};

class Compound;
class MetaboliteObject;
class Pool;
class MyAlgorithms;

typedef std::pair<MetaboliteObject, unsigned int> T_equationMetabolite;
typedef std::vector<T_equationMetabolite>::iterator T_equationMetaboliteIterator;


class Equation
{
public:
    Equation(void);
    //Equation(const Equation &E);
    Equation(MyAlgorithms *ma, std::string formula, std::set<std::string> *availableCompartments, 
        std::map<std::string, MetaboliteObject*> *metabolitesPool);
    Equation(MyAlgorithms *ma, std::string formula, bool *,std::set<std::string> *availableCompartments, 
        std::map<std::string, MetaboliteObject*> *metabolitesPool);
    Equation(MyAlgorithms *ma, const std::multimap<Compound*, unsigned int>&, 
        std::map<std::string, MetaboliteObject*> *metabolitesPool);
    ~Equation(void);
    std::string getLastError(void);
    bool parseEquation(MyAlgorithms *ma, std::string, std::set<std::string> *availableCompartments, std::map<std::string, MetaboliteObject*> *metabolitesPool);
    void parseEquation(MyAlgorithms *ma, const std::multimap<Compound*, unsigned int>& equation, std::map<std::string, MetaboliteObject*> *metabolitesPool);
    std::string text(bool withCompartments=false, bool withNames=false, Pool* pool=0);
    //std::string textWithNames(Pool *, bool withCompartments=false);
    bool isValid(void);
    bool hasMetaboliteCompartments(void);
    bool containsMetabolite(const T_equationMetabolite &searchMetabolite);
    void clear(void);
    void setStiochiometryType(const StoichiometryType& type);
    void setCompartmentType(const CompartmentType& type);
    unsigned int position(const T_equationMetabolite& equationMetabolite);
    /*
    void createOrLinkMetaboliteObjects(map<string, MetaboliteObject> *metabolitesPool, Compound *compounds);
    void calculateReachableMetabolites(MetaboliteObject *startMetabolite, 
        set<MetaboliteObject*> reachableMetabolites, bool withCompartments=false);
    */
    class MyAlgorithms *myAlgorithms;

    //std::vector<std::pair<std::string, unsigned int>> Compounds;
    //std::map<std::string, std::string> compartments;
    std::set<std::string> globalCompartments;
    DirectionType directionType;
    std::string rawEquation;
    std::vector<T_equationMetabolite> metabolites;
private:
    //std::string getCompoundName(std::string cNumber, Pool *);
    bool valid;
    std::string errorMessage;
    enum StoichiometryType stoichiometryType;
    enum CompartmentType compartmentType;
};
