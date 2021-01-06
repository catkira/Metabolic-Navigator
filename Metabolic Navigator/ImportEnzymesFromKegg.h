#pragma once
#include <QObject>
#include <string>
#include <vector>

class Pool;
class MyAlgorithms;

class ImportEnzymesFromKegg : public QObject
{
Q_OBJECT

public:
    ImportEnzymesFromKegg(std::string& text, Pool *pool, MyAlgorithms *myAlgorithms, unsigned int& count);
    ~ImportEnzymesFromKegg(void);
    void parseOrganismNames(const std::string& text);
    void buildOrganisms(void);
    Pool *pool;
    MyAlgorithms *myAlgorithms;
    unsigned int& count;

signals:
    void requestUpdate(unsigned int);

};
