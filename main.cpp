#include "mainwindow.h"
#include <QApplication>
#include <QImage>
#include <QFile>
#include <QString>
#include <QTextStream>
#include "pietcore.h"
#include "widgettest.h"
#include "executingpietlibraries.h"
#include <iostream>
#include <QLibrary>


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
    ExecutingPietLibraries::Hash_FuncSet();
    PietCore::rootpath = (QFileInfo (argv[0])).absolutePath() + QDir::separator();

    if(argc >= 2 ){
        auto loadedimage = QImage(argv[1]);
        if(loadedimage.isNull() ){cout << "Invalid Image! " << endl; return 0;}

        QString buffer(""); //I/0 処理
        QTextStream qstdin(stdin);
        qstdin.setCodec("UTF-8");
        PietCore core( [](QString outstr){
             cout << outstr.toStdString() << flush;
        },[&buffer,&qstdin](){
            if(buffer.isEmpty()) {buffer = qstdin.read(1);} //バッファに一文字読み込む
            QChar gotchar = buffer.at(0);
            buffer.remove(0,1);
            return QChar(gotchar);
        },[&buffer,&qstdin](bool &Miss){
            if(buffer.isEmpty()) {buffer = qstdin.readLine();}
            QRegExp rx("^\\s*-?\\d+");
            if(-1 == rx.indexIn(buffer)) { Miss = true; return 0;}
            QString Match = rx.cap();
            int m = Match.toInt();
            buffer.remove(0,Match.length());
            return m;
        } );

        core.setImage(loadedimage,argv[1]);
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
