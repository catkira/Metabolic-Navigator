#include "selectlabeldialog.h"

SelectLabelDialog::SelectLabelDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
}

SelectLabelDialog::~SelectLabelDialog()
{

}

void SelectLabelDialog::updateGUI(void)
{
    ui.listWidget->clear();
    QString temp;
    QListWidgetItem *item;
    for(unsigned int n=0;n<labelVector.size();n++)
    {
        item = new QListWidgetItem();
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        if(labelVector[n]!=0)
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
        temp = "C" + QString().setNum(n+1);
        item->setText(temp);
        ui.listWidget->addItem(item);
    }
}

void SelectLabelDialog::on_pushButton_2_clicked()
{
    QListWidgetItem *item;
    bool allZero = true;
    for(unsigned int n=0;n<labelVector.size();n++)
    {
        item = ui.listWidget->item(n);
        if(item->checkState() == Qt::Checked)
        {
            labelVector[n] = 1;
            allZero = false;
        }
        else
            labelVector[n] = 0;
    }
    if(allZero)
    {
        labelVector.resize(0);
        labelVector.clear();
    }
    close();
}

void SelectLabelDialog::on_pushButton_clicked()
{
    close();
}