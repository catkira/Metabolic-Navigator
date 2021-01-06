#include "transitionview.h"
#include <string>
#include "CompoundTransition.h"

using namespace std;

TransitionView::TransitionView(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
}

TransitionView::~TransitionView()
{

}

void TransitionView::selectTransition(CompoundTransition *transition)
{
    string temp;
    transition->generateText(temp);
    ui.textEdit->setText(temp.c_str());
    transition->generateIDText(temp);
    setWindowTitle(temp.c_str());

    transition->generateIDText(temp);
    ui.lineEdit->setText(temp.c_str());
}