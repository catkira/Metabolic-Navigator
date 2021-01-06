#pragma once
#include <string>
#include <vector>

class Pool;
class MyAlgorithms;


class ImportOrganism
{
public:
    ImportOrganism(std::string& text, Pool *pool, MyAlgorithms *myAlgorithms, unsigned int& count);
    ~ImportOrganism(void);
private:
    Pool *pool;
    MyAlgorithms *myAlgorithms;
    unsigned int& count;
};
