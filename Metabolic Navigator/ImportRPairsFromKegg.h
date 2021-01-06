#pragma once

#include <string>
#include <vector>

class Pool;
class MyAlgorithms;
class CompoundTransition;
class CompoundDefinition;

class ImportRPairsFromKegg
{
public:
    ImportRPairsFromKegg(std::string& text, Pool *pool, MyAlgorithms *myAlgorithms, unsigned int& count);
    ~ImportRPairsFromKegg(void);

    Pool *pool;
    MyAlgorithms *myAlgorithms;
    unsigned int& count;
private:
    //void createTransitionMatrices(void);
    std::string cur_compound1,cur_compound2,cur_name,cur_align,cur_entry1,cur_entry2,cur_compound1Name,cur_compound2Name;
    std::vector<std::string> cur_reactionIDs;
    void calculateMatrix(CompoundTransition& compoundTransition);

friend class AddEntry;
};
