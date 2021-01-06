#pragma once
#include <QObject>
#include <string>
#include <vector>

class Pool;
class MyAlgorithms;

class ImportReactionsFromKegg : public QObject
{
    Q_OBJECT
public:
    ImportReactionsFromKegg::ImportReactionsFromKegg(std::string& text, Pool *pool, MyAlgorithms *myAlgorithms, unsigned int& count);
    ~ImportReactionsFromKegg(void);
    void generateDefaultKeggOrganisms(void);
    Pool *pool;
    MyAlgorithms *myAlgorithms;
    unsigned int& count;
private:

signals:
    void requestUpdate(unsigned int);
};
