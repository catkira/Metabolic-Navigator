#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>
#include "ui_mylineedit.h"

class MyLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    MyLineEdit(QWidget *parent = 0);
    ~MyLineEdit();

private:
    Ui::MyLineEditClass ui;
    void keyPressEvent(QKeyEvent *event);
signals:
    void returnPressed(void);
};

#endif // MYLINEEDIT_H
