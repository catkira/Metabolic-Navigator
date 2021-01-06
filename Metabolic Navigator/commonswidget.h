#ifndef COMMONSWIDGET_H
#define COMMONSWIDGET_H

#include <QDockWidget>
#include "ui_commonswidget.h"

class CommonsWidget : public QDockWidget
{
    Q_OBJECT

public:
    CommonsWidget(QWidget *parent = 0);
    ~CommonsWidget();

private:
    Ui::CommonsWidgetClass ui;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_5_clicked();
    void on_groupBox_toggled(bool);

signals:
    void commonAction(QString);
};

#endif // COMMONSWIDGET_H
