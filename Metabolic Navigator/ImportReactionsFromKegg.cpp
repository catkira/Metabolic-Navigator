#include <map>
#include "ImportReactionsFromKegg.h"
#include "Pool.h"
#include "Organism.h"
#include "Reaction.h"
#include "defines.h"

using namespace std;

ImportReactionsFromKegg::ImportReactionsFromKegg(std::string& text, Pool *pool, MyAlgorithms *myAlgorithms, unsigned int& count)
: count(count), pool(pool), myAlgorithms(myAlgorithms)
{
}

ImportReactionsFromKegg::~ImportReactionsFromKegg(void)
{
}


void ImportReactionsFromKegg::generateDefaultKeggOrganisms(void)
{
    Organism organism;
    organism.setName("KEGG");
    T_reactionsIterator reactionsIterator;

    
    reactionsIterator = pool->reactions->begin();
    for(unsigned int n=0;n<pool->reactions->size();n++)
    {
        if(reactionsIterator->second.dataset == -1)
        {
            organism.reactions.insert(&(reactionsIterator->second));
        }
        ++reactionsIterator;
    }

    if(pool->organisms.find(organism.name()) == pool->organisms.end())
        pool->organisms.insert(pair<std::string, Organism>(organism.name(),organism));

    emit requestUpdate(UPDATE_ORGANISMS);

}
