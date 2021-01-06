#ifndef TRANSITIONVIEW_H
#define TRANSITIONVIEW_H

#include <QDialog>
#include "ui_transitionview.h"


class CompoundTransition;

class TransitionView : public QDialog
{
    Q_OBJECT

public:
    TransitionView(QWidget *parent = 0);
    ~TransitionView();
    void selectTransition(CompoundTransition *transition);

private:
    Ui::TransitionViewClass ui;
};

#endif // TRANSITIONVIEW_H
