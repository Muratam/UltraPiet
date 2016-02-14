#include "mainwindow.h"
#include <QApplication>
#include "widgettest.h"
#include <QFile>
#include <QTextStream>
#include "pietcore.h"
#include <iostream>
using namespace std;
void ApplyDarkStyleSheet(QApplication& a ){
    QFile f(":qdarkstyle/style.qss");
    if (f.exists()) {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        a.setStyleSheet(ts.readAll());
    }
}

int main(int argc, char *argv[]){
    cout << "Welcome to UltraPiet !" << endl;
    QApplication a(argc, argv);
    ApplyDarkStyleSheet(a);
    MainWindow w;
    w.show();
    return a.exec();
}
//auto wg = new WidgetTest(&w);
//wg->show();
