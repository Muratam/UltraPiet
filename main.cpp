#include "mainwindow.h"
#include <QApplication>
#include "widgettest.h"
#include <QFile>
#include <QTextStream>

void ApplyDarkStyleSheet(QApplication& a ){
    QFile f(":qdarkstyle/style.qss");
    if (f.exists()) {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        a.setStyleSheet(ts.readAll());
    }
}

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    ApplyDarkStyleSheet(a);
    MainWindow w;
    w.show();
    //auto wg = new WidgetTest(&w);
    //wg->show();
    return a.exec();
}
