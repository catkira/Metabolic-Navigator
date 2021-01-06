#pragma once


#include <string>
#include <vector>

class Pool;
class MyAlgorithms;

class ImportCompoundsFromKegg
{
public:
    ImportCompoundsFromKegg(std::string& text, Pool *pool, MyAlgorithms *myAlgorithms, unsigned int& count);
    ~ImportCompoundsFromKegg(void);
    Pool *pool;
    MyAlgorithms *myAlgorithms;
    unsigned int& count;
private:
};
