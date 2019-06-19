#include "mainwindow.h"
#include <QApplication>
//#include <map>
//#include <string>
//#include <iostream>

int main(int argc, char *argv[])
{

    //TIFF *tif = TIFFOpen

    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    //Dictionary in c++
//    std::map<std::string,double> m({{"value1",3.2},{"value2",4.5}});
//    m["value3"] = 5.6;
//    std::cout << m["value2"];

    return a.exec();
}
