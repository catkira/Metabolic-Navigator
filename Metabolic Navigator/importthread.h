#ifndef ImportThread_H
#define ImportThread_H

#include <QThread>
#include "Pool.h"
#include "MyAlgorithms.h"
#include <map>
#include <string>
#include <vector>
#include <QTimer>
#include "progressdialog.h"

class Pool;
class MyAlgorithms;
class Compound;

enum FunctionType
{
    ITF_COMPOUND,
    ITF_REACTION,
    ITF_RPAIR,
    ITF_SBML,
    ITF_MU,
    ITF_ENZYMES,
    ITF_ORGANISM
};

class ImportThread : public QThread
{
    Q_OBJECT

public:
    ImportThread(QObject *parent, FunctionType = ITF_COMPOUND);
    ~ImportThread();

    void run();

    std::map<std::string, Compound>::iterator compoundsIterator;
    class Pool *pool;
    class MyAlgorithms *myAlgorithms;

    std::vector<std::string> log;
    std::string bytes;
    std::string bytes2;
    std::string reactionBytes;
    std::string compoundBytes;
    ProgressDialog pDialog;
    unsigned int count;
    unsigned int endCount, endCount2;

private:
    void importCompounds(void);
    void importReactions(void);
    void importRPairs(void);
    void importSBML(void);
    void importMu(void);
    void importEnzymes(void);
    void importOrganism(void);

    FunctionType functionType_;
    QTimer updateTimer;
private slots:
    void updateCount(void);
    
signals:
    void finished(void);
    void addLogLine(QString);
    void PDsetInfoText(QString);
    void PDsetTitle(QString);
    void PDshow(void);
    void PDclose(void);
    void PDsetCurrentCount(unsigned int);
    void PDsetEndCount(unsigned int);
    void startUpdateTimer(void);
    void stopUpdateTimer(void);
    void requestUpdate(unsigned int);
};

#endif // ImportThread_H
