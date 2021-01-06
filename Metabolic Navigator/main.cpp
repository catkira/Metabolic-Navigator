#include <QtGui/QApplication>
#include "metabolicnavigator.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MetabolicNavigator w;
    w.show();
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    return a.exec();
}
