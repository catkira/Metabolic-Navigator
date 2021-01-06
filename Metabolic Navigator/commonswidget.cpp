#include "commonswidget.h"

CommonsWidget::CommonsWidget(QWidget *parent)
    : QDockWidget(parent)
{
    ui.setupUi(this);
}

CommonsWidget::~CommonsWidget()
{

}


void CommonsWidget::on_groupBox_toggled(bool)
{

}

void CommonsWidget::on_pushButton_5_clicked()
{
    emit commonAction("keggCompounds");
}

void CommonsWidget::on_pushButton_13_clicked()
{
    emit commonAction("keggReactions");
}

void CommonsWidget::on_pushButton_3_clicked()
{
    emit commonAction("keggRPairs");
}

void CommonsWidget::on_pushButton_2_clicked()
{
    emit commonAction("importKegg");
}

void CommonsWidget::on_pushButton_4_clicked()
{
    emit commonAction("importUser");
}

void CommonsWidget::on_pushButton_6_clicked()
{
    emit commonAction("createNetwork");
    
}

void CommonsWidget::on_pushButton_clicked()
{

}