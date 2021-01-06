#ifndef LABELPOSITIONASSISTENT_H
#define LABELPOSITIONASSISTENT_H

#include <QDockWidget>
#include "ui_labelpositionassistent.h"
#include <string>
#include <vector>
#include <QTimer>

class Pool;
class MyAlgorithms;
class MetabolicTree;
class Compound;
class Organism;
class Options;
class SvgViewer;
class MetabolitesViewer;


typedef std::vector<MetabolicTree>::iterator T_MetabolicTreesIterator;

class LabelPositionAssistent : public QDockWidget
{
    Q_OBJECT

public:
    LabelPositionAssistent(QWidget *parent = 0, Pool *pool = 0, MyAlgorithms *myAlgorithms = 0, Options *options = 0);
    ~LabelPositionAssistent();

    SvgViewer *svgViewer;
public slots:
    void update(unsigned int signal);

private:
    void updateNetworksTable(void);
    Ui::LabelPositionAssistentClass ui;
    Pool *pool;
    MyAlgorithms *myAlgorithms;
    Options *options;
    MetabolitesViewer *metabolitesViewer;

    MetabolicTree *metabolicTreeAllLabeled;
    std::vector<Compound*> endMetabolites;
    std::vector<Compound*> startMetabolites;
    Compound* endCompound;
    Compound* startCompound;
    Organism *selectedOrganism;

    std::vector<MetabolicTree> metabolicTrees;

    QTimer timer;

private slots:
    void on_pushButton_6_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_clicked();
    void on_comboBox_currentIndexChanged(int);
    void on_pushButton_2_clicked();
    void coordinateJobsAndUpdateStatus(void);
};

#endif // LABELPOSITIONASSISTENT_H
