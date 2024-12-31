#include <QApplication>
#include "ekran.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Ekran ekran;
    ekran.show();

    return a.exec();
}
