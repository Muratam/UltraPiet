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
        QString buffer("");
        QTextStream qstdin(stdin); //日本語入力バックスペースバグ…
        qstdin.setCodec("UTF-8");
        PietCore core( [](QString outstr){
             cout << outstr.toStdString() << flush; // 毎回flushすると遅くなるが…
        },[&buffer,&qstdin](){
            if(buffer.isEmpty()) {buffer = qstdin.read(1);} //バッファに一文字読み込む
            QChar gotchar = buffer.at(0);
            buffer.remove(0,1);
            return QChar(gotchar);
            //return qstdin.read(1).at(0);
        },[&buffer,&qstdin](bool &Miss){
            if(buffer.isEmpty()) {buffer = qstdin.readLine();}
            QRegExp rx("^\\s*-?\\d+");
            if(-1 == rx.indexIn(buffer)) { Miss = true; return 0;}
            QString Match = rx.cap();
            int m = Match.toInt();
            buffer.remove(0,Match.length());
            return m;
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
/*
        [inputWindow,this](){
            if(inputWindow->toPlainText().isEmpty())MSGBOX("NO INPUT CHARS")
            while(inputWindow->toPlainText().isEmpty()){
                this->isWaitingInput = true;
                QApplication::processEvents();
                if(isWaintingInputThenExecCancelSignal){
                    isWaitingInput = false;
                    return QChar(0);
                }
            }
            QString Input = inputWindow->toPlainText();
            QChar res = Input.at(0);
            Input.remove(0,1);
            inputWindow->setPlainText(Input);
            isWaitingInput = false;
            return res;
        },[inputWindow,this](bool &Miss){
            if(inputWindow->toPlainText().isEmpty())MSGBOX("NO INPUT CHARS")
            while(inputWindow->toPlainText().isEmpty()){
                this->isWaitingInput = true;
                QApplication::processEvents();
                if(isWaintingInputThenExecCancelSignal){
                    isWaitingInput = false;
                    Miss = true;
                    return 0;
                }
            }//空ではなければ処理
            QString Input = inputWindow->toPlainText();
            QRegExp rx("^\\s*-?\\d+");
            if(-1 == rx.indexIn(Input)) { Miss = true; return 0;}
            QString Match = rx.cap();
            int m = Match.toInt();
            Input.remove(0,Match.length());
            inputWindow->setPlainText(Input);
            isWaitingInput = false;
            return m;
        }
*/
