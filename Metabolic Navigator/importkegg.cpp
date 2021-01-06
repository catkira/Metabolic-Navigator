#include "importkegg.h"
#include <QDir>
#include <QFile>
#include <QTime>
#include <boost/spirit.hpp>



using namespace std;
using namespace boost::spirit;

ImportKegg::ImportKegg(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    importCompoundThread = new ImportThread(this, ITF_COMPOUND);
    importReactionThread = new ImportThread(this, ITF_REACTION);
    importRPairThread = new ImportThread(this, ITF_RPAIR);
    importEnzymesThread = new ImportThread(this, ITF_ENZYMES);

    connect(importCompoundThread, SIGNAL(addLogLine(QString)), this, SLOT(addLogLineSlot(QString)));
    connect(importCompoundThread, SIGNAL(requestUpdate(unsigned int)), this, SLOT(requestUpdateSlot(unsigned int)));
    connect(importCompoundThread, SIGNAL(finished()), this, SLOT(compoundFinished()));

    connect(importReactionThread, SIGNAL(addLogLine(QString)), this, SLOT(addLogLineSlot(QString)));
    connect(importReactionThread, SIGNAL(requestUpdate(unsigned int)), this, SLOT(requestUpdateSlot(unsigned int)));
    connect(importReactionThread, SIGNAL(finished()), this, SLOT(reactionFinished()));

    connect(importRPairThread, SIGNAL(addLogLine(QString)), this, SLOT(addLogLineSlot(QString)));
    connect(importRPairThread, SIGNAL(requestUpdate(unsigned int)), this, SLOT(requestUpdateSlot(unsigned int)));
    connect(importRPairThread, SIGNAL(finished()), this, SLOT(rpairFinished()));

    importReactions = false;
}

ImportKegg::~ImportKegg()
{
    delete importCompoundThread;
    delete importReactionThread;
    delete importRPairThread;
    delete importEnzymesThread;
}

void ImportKegg::compoundFinished(void)
{
    status = IMPORT_KEGG_SUCCESS;
    if(importReactions)
    {
        importReactions = false;
        importReaction();   
    }
}

void ImportKegg::importCompoundAndReaction(void)
{
    importReactions = true;
    importCompound();
}


void ImportKegg::reactionFinished(void)
{
    status = IMPORT_KEGG_SUCCESS;
    emit importFinished();
}

void ImportKegg::rpairFinished(void)
{
    status = IMPORT_KEGG_SUCCESS;

}


void ImportKegg::addLogLineSlot(QString text)
{
    emit addLogLine(text);
}

void ImportKegg::importEnzymes(void)
{
    if(importRPairThread->isRunning())
        return;
    QFile file;
    //QDir::setCurrent("C:/kegg/ligand/rpair/");
    QDir::setCurrent(QString(pool->dataDir.c_str()));
//    QDir::setCurrent(pool->keggDir.c_str() + QString("ligand\\enzymes\\"));
    file.setFileName("enzymes");
    if(file.open(QIODevice::ReadOnly)!=true)
    {
        emit addLogLine("could not open rpair");
        return;
    }
    importEnzymesThread->bytes = file.readAll().constData();
    file.close();

    unsigned int maxCount = 0;
    unsigned int pos = 0;
    while(pos != string::npos)
    {
        pos = importEnzymesThread->bytes.find("ENTRY ",pos);
        if(pos != string::npos)
            ++pos;
        else
            break;
        ++maxCount;
    }


    importEnzymesThread->pool = pool;
    importEnzymesThread->myAlgorithms = myAlgorithms;

    importEnzymesThread->pDialog.setEndCount(maxCount);

    importEnzymesThread->start();
}

void ImportKegg::importRPair(void)
{
    if(importRPairThread->isRunning())
        return;
    QFile file;
    //QDir::setCurrent("C:/kegg/ligand/rpair/");
    //QDir::setCurrent(pool->keggDir.c_str() + QString("ligand\\rpair\\"));
    QDir::setCurrent(pool->dataDir.c_str());
    file.setFileName("rpair");
    if(file.open(QIODevice::ReadOnly)!=true)
    {
        emit addLogLine("could not open rpair");
        return;
    }
    importRPairThread->bytes = file.readAll().constData();
    file.close();

    unsigned int maxCount = 0;
    unsigned int pos = 0;
    while(pos != string::npos)
    {
        pos = importRPairThread->bytes.find("ENTRY ",pos);
        if(pos != string::npos)
            ++pos;
        else
            break;
        ++maxCount;
    }


    importRPairThread->pool = pool;
    importRPairThread->myAlgorithms = myAlgorithms;

    importRPairThread->pDialog.setEndCount(maxCount);

    importRPairThread->start();
}


void ImportKegg::importCompound(void)
{
    if(importCompoundThread->isRunning())
        return;
    status = IMPORT_KEGG_BUSY;
    QFile file;
    //QDir::setCurrent("C:/kegg/ligand/compound/");
//    QDir::setCurrent(pool->keggDir.c_str() + QString("ligand\\compound\\"));
    QDir::setCurrent(pool->dataDir.c_str());

    file.setFileName("compound");
    if(file.open(QIODevice::ReadOnly)!=true)
    {
        emit addLogLine("could not open compound");
        return;
    }
    int val;
    unsigned int numCompounds=0;
    QTime time;
    time.start();
    importCompoundThread->bytes = file.readAll().constData();
    file.close();

    unsigned int maxCount = 0;

    unsigned int pos = 0;
    while(pos != string::npos)
    {
        pos = importCompoundThread->bytes.find("Compound",pos);
        if(pos != string::npos)
            ++pos;
        else
            break;
        ++maxCount;
    }


    importCompoundThread->pool = pool;
    importCompoundThread->myAlgorithms = myAlgorithms;
    

    importCompoundThread->pDialog.setEndCount(maxCount);

    importCompoundThread->start();
}


void ImportKegg::importReaction(void)
{
    if(importReactionThread->isRunning())
        return;

    status = IMPORT_KEGG_BUSY;


    QFile file;
    //QDir::setCurrent("C:/kegg/ligand/reaction/");
//    QDir::setCurrent(pool->keggDir.c_str() + QString("ligand\\reaction\\"));
    QDir::setCurrent(pool->dataDir.c_str());

    file.setFileName("reaction");
    if(file.open(QIODevice::ReadOnly)!=true)
    {
        emit addLogLine("could not open reaction");
        return;
    }
    int val;
    unsigned int numReactions=0;
    importReactionThread->bytes = file.readAll().constData();
    file.close();


    unsigned int maxCount = 0;

    unsigned int pos = 0;
    while(pos != string::npos)
    {
        pos = importReactionThread->bytes.find("ENTRY",pos);
        if(pos != string::npos)
            ++pos;
        else
            break;
        ++maxCount;
    }



    importReactionThread->pool = pool;
    importReactionThread->myAlgorithms = myAlgorithms;


    importReactionThread->pDialog.setEndCount(maxCount);

    importReactionThread->start();

}



string ImportKegg::getKeggValue(string *bytes, unsigned int start)
{
    unsigned int nextLineStart,lineStart;
    lineStart = start;
    string ret;
    do
    {
        nextLineStart = bytes->find('\n',lineStart)+1;    
        ret += bytes->substr(lineStart+12, nextLineStart-1 - lineStart - 12);
        lineStart = nextLineStart;
        if((*bytes)[lineStart]==' ')
            ret += " ";
    }while((*bytes)[lineStart]==' ');
    return ret;
}

void ImportKegg::checkForKeggFiles(void)
{
    QFile file;
    ui.listWidget_2->clear();
    QDir::setCurrent("C:/kegg/ligand/compound/");
    file.setFileName("compound");
    if(file.exists())
    {
        emit addLogLine("compound file available");
        ui.listWidget_2->addItem("compound");
    }
    QDir::setCurrent("C:/kegg/ligand/reaction/");
    file.setFileName("reaction");
    if(file.exists())
    {
        emit addLogLine("reaction file available");
        ui.listWidget_2->addItem("reaction");
    }
    QDir::setCurrent("C:/kegg/ligand/rpair/");
    file.setFileName("rpair");
    if(file.exists())
    {
        emit addLogLine("rpair file available");
        ui.listWidget_2->addItem("rpair");
    }
}


void ImportKegg::on_pushButton_1_clicked()
{
    checkForKeggFiles();
}

void ImportKegg::on_pushButton_2_clicked()
{
}

void ImportKegg::on_pushButton_5_clicked()
{
    QList<QListWidgetItem *> selectedItems;
    selectedItems = ui.listWidget_2->selectedItems();
    if(selectedItems.size()==0)
        return;
    for(unsigned int n=0;n<selectedItems.size();n++)
    {
        if(selectedItems[n]->text()=="reaction")
            importReaction();
        else if(selectedItems[n]->text()=="compound")
            importCompound();
        else if(selectedItems[n]->text() == "rpair")
            importRPair();
    }
    close();
}

void ImportKegg::requestUpdateSlot(unsigned int signal)
{
    emit requestUpdate(signal);
}
