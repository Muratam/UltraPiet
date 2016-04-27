#include "mainwindow.h"
#include "pietcore.h"
#include "executingpietlibraries.h"
#include <QApplication>
#include <QImage>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <iostream>
#include <stdlib.h>

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
    QApplication a(argc, argv);
   // return connectSlack(QString("xoxp"),QString("#murata-memo"),QString("hello"));

    if(argc >= 2 ){
        auto loadedimage = QImage(argv[1]);
        if(loadedimage.isNull() ){cout << "Invalid Image! " << endl; return 0;}

        bool outputMediumFile = false;
        if(argc >= 3 && argv[2][0] == '-' && argv[2][1] == 'm' )outputMediumFile = true;

        QString buffer(""); //I/0 処理
        QTextStream qstdin(stdin);
        QTextStream qstdout(stdout);
        qstdin.setCodec("UTF-8");
        qstdout.setCodec("UTF-8");

        PietCore core( [&qstdout](QString outstr){
            qstdout << outstr ;
            qstdout.flush();
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
        } , outputMediumFile);

        core.setImage(loadedimage,argv[1]);
        core.exec();
        cout << endl;

        return 0;
    }else{
        cout << "Welcome to UltraPiet !" << endl;
        ApplyDarkStyleSheet(a);
        MainWindow w;
        w.show();
        return a.exec();
    }
}


