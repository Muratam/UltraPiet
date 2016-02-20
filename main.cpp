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
#include <thread>
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

void midiOutOpenTest(){
    vector<PietTree> pts{ PietTree(0), PietTree(-1),PietTree(0), PietTree(0), PietTree(0)};
    bool Miss = false;

    PietTree ptsres = ExecutingPietLibraries::LoadDLL(Miss,QString("Winmm"),QString("midiOutOpen"),QString ("NiViiu"),pts);
    if(!Miss) pts.push_back(ptsres);
    if(pts[pts.size()-1].Val() != 0) return;
    pts.pop_back();
    pts[pts.size()-1].flatten();
    int p = pts[pts.size() -1].Nodes()[0].Val();
    pts.pop_back();

    pts.clear();
    pts.push_back(PietTree(p));
    pts.push_back(PietTree((0xc << 4) | 0 | (0 << 8) | (0 << 16)));
    PietTree ptsressm = ExecutingPietLibraries::LoadDLL(Miss,QString("Winmm"),QString("midiOutShortMsg"),QString ("iiu"),pts);
    pts.clear();
    pts.push_back(PietTree(p));
    pts.push_back(PietTree((0x9 << 4) | 0 | (0x45 << 8) | (40 << 16)));
    ptsressm = ExecutingPietLibraries::LoadDLL(Miss,QString("Winmm"),QString("midiOutShortMsg"),QString ("iiu"),pts);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    pts.clear();
    pts.push_back(PietTree(p));
    pts.push_back(PietTree((0x9 << 4) | 0 | (0x45 << 8) | (0 << 16)));
    ptsressm = ExecutingPietLibraries::LoadDLL(Miss,QString("Winmm"),QString("midiOutShortMsg"),QString ("iiu"),pts);


    PietTree ptsres2 = ExecutingPietLibraries::LoadDLL(Miss,QString("Winmm"),QString("midiOutReset"),QString ("iu"),pts);
    PietTree ptsres3 = ExecutingPietLibraries::LoadDLL(Miss,QString("Winmm"),QString("midiOutClose"),QString ("iu"),pts);

}


PietTree MessageBoxATest(bool& Miss){
    vector<PietTree> pts{ PietTree(0), PietTree(QString("hello! dll world! ")), PietTree(QString("title")), PietTree(0) };
    return ExecutingPietLibraries::LoadDLL(Miss,QString("user32"),QString("MessageBoxA"),QString ("VCCui"),pts);
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
