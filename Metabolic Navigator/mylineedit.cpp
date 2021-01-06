#include "mylineedit.h"

MyLineEdit::MyLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    ui.setupUi(this);
}

MyLineEdit::~MyLineEdit()
{

}

void MyLineEdit::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter)
        emit returnPressed();
    QLineEdit::keyPressEvent(event); 
}