#pragma once
#include <QObject>
#include <map>
#include <vector>
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "pool.h"
#include "defines.h"
#include "boost/numeric/ublas/vector.hpp"

class Compound;
class Reaction;
class TreeNode;
class Equation;

typedef std::pair<MetaboliteObject, unsigned int> T_equationMetabolite;
typedef std::vector<std::pair<MetaboliteObject, unsigned int>>::iterator T_equationMetabolitesIterator;



class MyAlgorithms : public QObject
{
    Q_OBJECT
private:
     void addIfCompartmentMatches(const MetaboliteObject& startMetabolite,
            unsigned int startMetabolitePosition,
            T_equationMetabolite newEquationMetabolite,
            Reaction *reaction,
            std::map<MetaboliteObject *, std::vector<Reaction *>> *targetMetaboliteLinkedReactions,
            std::map<Reaction *, std::vector<MetaboliteObject*>> *targetReactionLinkedMetabolites,
            bool withCompartments);
    void findCompoundByName(const std::string& pattern);
    void findCompoundById(const std::string& pattern);
    bool linkedReactionsDirty;

public:
    MyAlgorithms(void);
    ~MyAlgorithms(void);
    void calculateIsomeres();
    void calculateMissingRpairs(void);
    void findReplaceableMetabolites(void);

    void findCompoundByFormula(const std::string& pattern);

    void findCompounds(const QStringList& pattern, 
        std::set<Compound*>& foundCompounds, bool& unique, bool append=false);

    void calculateLinkedReactions(void);
    void calculateLinkedRPairs(void);

    void calculatePossibleNewMetabolites(MetaboliteObject const*  startMetabolite, 
        const std::set<Reaction*>* const reactionSet, 
        std::map<MetaboliteObject *, std::vector<Reaction *>> *targetMetaboliteLinkedReactions,
        std::map<Reaction *, std::vector<MetaboliteObject*>> *targetReactionLinkedMetabolites,
        bool withCompartments=false,
        unsigned int rpairsOption = 0,
        bool reverse=false);
    void fixCompartments(void);
    void addRpairsFromKeggToUserReactions(void);
    MetaboliteObject* getOrCreateMetaboliteObject(Compound *compound, 
        std::string compartment, bool create=true);
    MetaboliteObject* getOrCreateMetaboliteObject(Compound *compound, 
        std::string compartment, const boost::numeric::ublas::vector<double>& label, bool create=true);
     
    Compound* getOrCreateCompound(std::string id);
    
    void buildReactionSet(std::vector<Reaction*> *reactionSet,
                      bool includeUserReactions,
                      bool includeNotAccepted,
                      bool includeTransporter,
                      bool includeKegg);

    void findMetabolites(const QStringList& pattern, 
        std::vector<MetaboliteObject*>& foundMetabolites,
        bool append=false);

    int findReactions(
        std::vector<T_equationMetabolite>& equationMetabolites, 
        unsigned int eqnDirection, const std::set<Reaction*>& reactionSet,
        const bool& append = false);

    void setLinkedReactionsDirty(void)
    {
        linkedReactionsDirty = true;
    }
    void validateLinkedReactions(void)
    {
        if(linkedReactionsDirty)
            calculateLinkedReactions();
    }
    void validateLinkedRPairs(void)
    {
        calculateLinkedRPairs();
        linkedReactionsDirty = false;
    }

    void parseReactionUserSettings(std::string text);

    void triggerAddLogLineSignal(const std::string& text);

    class Pool *pool;

    std::map<unsigned int, unsigned int> *reachableCompounds;

    std::string tempReactionID;
    std::string tempReactionSetting;
signals:
    void addLogLine(QString);
    void updateCompartmentList(void);

};

QString getCompoundFromMetabolite(const QString& pattern);
QString getCompartmentFromMetabolite(const QString& pattern);

void twistLeftRightInEquationMetabolites(std::vector<T_equationMetabolite>& equationMetabolites);


void MetabolitesToEquationMetabolites(const std::vector<MetaboliteObject*> metabolites, 
    std::vector<T_equationMetabolite>& equationMetabolites, 
    const unsigned int& value, const bool& append = false);
