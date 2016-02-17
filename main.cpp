#include "mainwindow.h"
#include <QApplication>
#include <QImage>
#include <QFile>
#include <QString>
#include <QTextStream>
#include "pietcore.h"
#include "widgettest.h"
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
    if(argc >= 2 ){
        auto loadedimage = QImage(argv[1]);
        if(loadedimage.isNull() ){cout << "Invalid Image! " << endl; return 0;}
        PietCore core(
          [](QString outstr){ cout << outstr.toStdString();
        },[](){
            return QChar(0);
        },[](bool &Miss){
            return 0;
        } );
        core.setImage(loadedimage);
        core.exec();
        cout << endl;
        return 0;
    }else{
        cout << "Welcome to UltraPiet !" << endl;
        QApplication a(argc, argv);
        ApplyDarkStyleSheet(a);
        MainWindow w;
        w.show();
        return a.exec();
    }
}
//auto wg = new WidgetTest(&w);
//wg->show();

