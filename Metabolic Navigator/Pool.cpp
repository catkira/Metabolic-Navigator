#include "Pool.h"

#include "CompoundTransition.h"
#include "CompoundDefinition.h"
#include "UserDataset.h"
#include "Compound.h"
#include "Equation.h"
#include "Reaction.h"
#include "Enzyme.h"
#include "MyAlgorithms.h"
#include "MetaboliteObject.h"
#include "Organism.h"
#include "FateMapCompound.h"

using namespace std;

Pool::Pool(void)
{
    userDatasets = new map<unsigned int, UserDataset>;
    compounds = new map<string, Compound>; 
    compoundNames = new map<string, Compound*>;
    equations = new map<unsigned int, Equation>;
    reactions = new map<std::string, Reaction>;
    enzymes = new vector<Enzyme>;
    //selectedReactions = new map<unsigned int, unsigned int>;
    selectedReactions2 = new set<Reaction*>;
    selectedCompounds = new set<string>;
    metabolitesPool = new map<string, MetaboliteObject*>;
    availableCompartments = new set<string>;
}

Pool::~Pool(void)
{
// todo: cleanup organisms

    delete userDatasets;
    delete compounds;
    delete compoundNames;
    delete equations;
    delete reactions;
    //delete selectedReactions;
    delete selectedReactions2;
    delete selectedCompounds;
    delete metabolitesPool;
    delete availableCompartments;
    delete enzymes;
}
