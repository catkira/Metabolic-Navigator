#include "calculatelinkedreactions.h"

CalculateLinkedReactions::CalculateLinkedReactions(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
}

CalculateLinkedReactions::~CalculateLinkedReactions()
{

}


void CalculateLinkedReactions::on_pushButton_clicked()
{
    myAlgorithms->calculateLinkedReactions();
    close();
}