#ifndef PALSSONKEGGCONVERTER_H
#define PALSSONKEGGCONVERTER_H

#include <QDialog>
#include "ui_palssonkeggconverter.h"
#include <map>
#include <string>
#include "equation.h"

class Pool;
class MyAlgorithms;

class ReactionPalsson
{
public:
    std::string id;
    std::vector<std::string> leftCompounds;
    std::vector<std::string> rightCompounds;
    DirectionType directionType;
};

class PalssonKeggConverter : public QDialog
{
    Q_OBJECT

public:
    PalssonKeggConverter(QWidget *parent = 0, Pool *pool=0, MyAlgorithms *myAlgorithms = 0);
    ~PalssonKeggConverter();


private:
    void updateTable(void);
    Ui::PalssonKeggConverterClass ui;
    Pool *pool;
    MyAlgorithms *myAlgorithms;
    ReactionPalsson reactionPalsson;

    std::multimap<std::string,std::string> palssonKeggCompounds;
    std::multimap<std::string,std::string> keggPalssonCompounds;
    std::map<std::string, double> fluxData;
    std::map<std::string, std::pair<std::string,int>> palssonKeggMapping;
private slots:
    void on_pushButton_4_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();

friend class AddPalssonReaction;
};

#endif // PALSSONKEGGCONVERTER_H
