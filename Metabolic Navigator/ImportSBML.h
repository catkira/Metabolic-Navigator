#pragma once
#include <string>
#include <QObject>

class Pool;
class MyAlgorithms;

class ImportSBML : public QObject
{
    Q_OBJECT
public:
    ImportSBML(const std::string& fileName, Pool *pool, MyAlgorithms *myAlgorithms, unsigned int& count);
    ~ImportSBML(void);
    void generateDefaultSBMLOrganisms(std::string name);
private:
    MyAlgorithms *myAlgorithms;
    Pool *pool;
    std::string fileName;
signals:
    void requestUpdate(unsigned int);
};
