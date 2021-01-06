#ifndef IMPORTKEGG_H
#define IMPORTKEGG_H

#include <QDialog>
#include "ui_importkegg.h"
#include "Compound.h"
#include "Reaction.h"
#include <map>
#include <string>
#include "MetaboliteObject.h"
#include "pool.h"
#include "MyAlgorithms.h"
#include "ImportThread.h"
#include <QTimer>

enum ImportKeggStatus
{
    IMPORT_KEGG_BUSY,
    IMPORT_KEGG_FAILED,
    IMPORT_KEGG_SUCCESS
};

class ImportKegg : public QDialog
{
    Q_OBJECT

public:
    ImportKegg(QWidget *parent = 0);
    ~ImportKegg();
    void checkForKeggFiles(void);
    void importReaction(void);
    void importCompound(void);
    void importRPair(void);
    void importEnzymes(void);
    void importCompoundAndReaction(void);
    std::map<std::string, Compound>::iterator compoundsIterator;
    std::map<unsigned int, Reaction>::iterator reactionsIterator;
    class Pool *pool;
    class MyAlgorithms *myAlgorithms;

    ImportKeggStatus status;

private:
    Ui::ImportKeggClass ui;

    std::string getKeggValue(std::string *bytes, unsigned int start);

    ImportThread *importCompoundThread;
    ImportThread *importReactionThread;
    ImportThread *importRPairThread;
    ImportThread *importEnzymesThread;

    bool importReactions;

private slots:
    void on_pushButton_5_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_1_clicked();
    void addLogLineSlot(QString);
    void requestUpdateSlot(unsigned int);
    void compoundFinished(void);
    void reactionFinished(void);
    void rpairFinished(void);

signals:
    void addLogLine(QString);
    void updateCompartmentList(void);
    void requestUpdate(unsigned int);
    void importFinished(void);
};

#endif // IMPORTKEGG_H
