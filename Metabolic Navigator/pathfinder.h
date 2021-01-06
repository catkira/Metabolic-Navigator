#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <QDialog>
#include <QString>
#include <map>
#include <string>
#include "ui_pathfinder.h"
#include "MetabolicPath.h"
#include "defines.h"
#include "boost/numeric/ublas/vector.hpp"

class MetabolicTree;
class Options;
class MetabolicPathViewer;
class MyAlgorithms;
class TreeNode;
class SvgViewer;
class Pool;
class Organism;
class QTreeWidgetItem;

class PathFinder : public QDialog
{
    Q_OBJECT

public:
    PathFinder(QWidget *parent = 0, Options *options=0);
    ~PathFinder();
    void showCompartmentsInTreeView(void);

    Options *options;
    MetabolicPathViewer *metabolicPathViewer;
    std::map<std::string, unsigned int> metaboliteKeysLevel;
    Pool *pool;
    MyAlgorithms *myAlgorithms;

public slots:
    void updateCompartmentList(void);
    void update(unsigned int);
    void nodeDoubleClicked(const QString& nodeId, QPoint);
    void nodeRightClicked(const QString& nodeId, QPoint);


private:
    std::list<MetabolicPath<Reaction, MetaboliteObject>> findPaths(TreeNode *node, MetabolicPath<Reaction,MetaboliteObject> path);
    bool addToCompartmentTreeView(std::string compartment, std::string outside, QTreeWidgetItem* twi);
    Ui::PathFinderClass ui;
    unsigned int maxPathLength;
    TreeNode *endNode;
    TreeNode *startNode;
    SvgViewer *svgViewer;
    MetabolicTree *tree;
    Organism *selectedOrganism;
    boost::numeric::ublas::vector<double> startLabel;

    QMenu *menu;
    QMenu *menuChangeName;
    QMenu *menuNeighbourNodes;

private slots:
    void on_pushButton_6_clicked();
    void on_pushButton_5_clicked();
    void on_comboBox_4_currentIndexChanged(int);
    void on_comboBox_2_currentIndexChanged(QString);
    void on_comboBox_2_editTextChanged(QString);
    void on_pushButton_4_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
signals:
    void addLogLine(QString);
};

#endif // PATHFINDER_H
