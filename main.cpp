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
    /*
    vector<PietTree> pts{
        PietTree(0),
        PietTree(QString("hello! dll world")),
        PietTree(QString("title")),
        PietTree(0)
    };
    bool Miss = false;
    PietTree pt = ExecutingPietLibraries::LoadDLL(Miss,QString("user32"),QString("MessageBoxA"),QString ("VCCui"),pts);
    return 0;
    */
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

/*
//static extern uint midiOutOpen(out IntPtr lphMidiOut, uint uDeviceID, IntPtr dwCallback, IntPtr dwInstance, uint dwFlags);
//this is void* => void MidiOutProc(void* hmo, unsigned int hwnd, int dwInstance, int dwParam1, int dwParam2);
//enum class CALLBACKMIDI { EVENT = 0x50000, FUNCTION = 0x30000, NULL = 0x0, THREAD = 0x20000, WINDOW = 0x10000, };
//

unsigned int midiOutOpen(void* lphmo, int uDeviceID, void* dwCallback, int dwCallbackInstance, int dwFlags){
    return retLib<unsigned int>(QString("Winmm"),QString("midiOutShortMsg"),lphmo,uDeviceID, dwCallback ,dwCallbackInstance,dwFlags);
}//this lphmo may be changeable
unsigned int midiOutShortMsg(void* hmo, int dwMsg){
    return retLib<unsigned int>(QString("Winmm"),QString("midiOutShortMsg"),hmo,dwMsg);}
unsigned int midiOutShortMsg(void* hmo,unsigned char status,unsigned char channel,unsigned char d1,unsigned char d2) {
    midiOutShortMsg(hmo, (status << 4) | channel | (d1 << 8) | (d2 << 16));}
unsigned int midiOutReset(void* hmo){
    return retLib<unsigned int>(QString("Winmm"),QString("midiOutReset"),hmo);}
unsigned int midiOutClose(void* hmo){
    return retLib<unsigned int>(QString("Winmm"),QString("midiOutClose"),hmo);}
void LangA(int times) {
    void* hMidi ;
    midiOutOpen (hMidi, -1, nullptr, 0, 0);
    midiOutShortMsg(hMidi, 0xc, 0, 0 , 40);
    //REP (i,times) {
    //    midiOutShortMsg(hMidi, 0x9, 0, 0x45, 40);
    //    cout << "langing" << endl;Sleep(1000);
    //    midiOutShortMsg(hMidi, 0x9, 0, 0x45, 0);
    //}
    midiOutReset(hMidi);
    midiOutClose(hMidi);
}


*/
