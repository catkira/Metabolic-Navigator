#ifndef SELECTLABELDIALOG_H
#define SELECTLABELDIALOG_H

#include <QDialog>
#include "ui_selectlabeldialog.h"
#include "boost/numeric/ublas/vector.hpp"
#include "boost/numeric/ublas/io.hpp"


class SelectLabelDialog : public QDialog
{
    Q_OBJECT

public:
    SelectLabelDialog(QWidget *parent = 0);
    ~SelectLabelDialog();
    void setVector(const boost::numeric::ublas::vector<double> &vec){
        labelVector = vec;
        updateGUI();
    }
    boost::numeric::ublas::vector<double> getVector(void){
        return labelVector;
    };


private:
    Ui::SelectLabelDialogClass ui;
    boost::numeric::ublas::vector<double> labelVector;
    void updateGUI(void);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // SELECTLABELDIALOG_H
