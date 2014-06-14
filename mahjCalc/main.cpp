#include <QApplication>

#include "calculator.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MahjCalc calc;
    calc.show();
    return app.exec();
}
