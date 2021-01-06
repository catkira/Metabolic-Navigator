#pragma once
#include <QObject>
#include <map>
#include <string>
#include <vector>

class Pool;
class MyAlgorithms;
class CompoundTransition;

namespace mu
{

class ReactionMu
{
public:
    std::string name;
    std::string ecnumber;
    std::string inchi;
    std::string rnumber;
    std::string link;
    //std::multimap<std::string, std::pair<int,int>> mappings;
};

class ImportFateMaps 
{
public:
    ImportFateMaps(Pool *pool, MyAlgorithms *myAlgorithms, unsigned int& count);

    ~ImportFateMaps(void);
    void readFiles(const std::string& dir);
    void readFiles(const std::string& compounds, const std::string& reactions);
    void readCompounds(const std::string& compounds);
    void readReactions(const std::string& compounds);
    Pool *pool;
    MyAlgorithms *myAlgorithms;
    unsigned int& count;

private:
    class ReactionMu reactionMu;
    std::map<std::string, std::string> inchiCnumberMap;
friend class AddEntry;
};

}