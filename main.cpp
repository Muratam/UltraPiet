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

int PlayHangGliderGame(){
    //たけしのハンググライダー
    //１　主人公はハンググライダー。操作は上下移動のみ。
    //２　敵は鳥。左にしか進まない。あたると即死
    //３　nループに一度縦方向のどこかに現れるのでよけ続ける
    //Me.png,Bird.png,Back.png,Sky.png,GameOver.pngのみでよい
    //(自分の移動),(敵の描画),(ロジック),(ゲームオーバー処理)
    //   50*50      ,  50*50   , 600 * 400 , 600 * 400
    // [y] ,[[x,y],...],LoopCount,KeyPushed

    const int w = 600, h = 400;
    const int size = 50;
    const int meX = 50;
    const int birdemitcount = 1000;
    const int birdspeed = 1;
    vector<PietTree>pts;

    pts = {PietTree(w),PietTree(h),PietTree(QString("Hang Glider Game"))};
    ExecutingPietLibraries::functionHash[QString("MakeGLView")](pts);

    pts = {PietTree(QString("Me.png"))};
    ExecutingPietLibraries::functionHash[QString("GLLoadImage")](pts);
    int meH = pts[0].Val();

    pts = {PietTree(QString("Bird.png"))};
    ExecutingPietLibraries::functionHash[QString("GLLoadImage")](pts);
    int birdH = pts[0].Val();

    pts = {PietTree(QString("Sky.png"))};
    ExecutingPietLibraries::functionHash[QString("GLLoadImage")](pts);
    int skyH = pts[0].Val();

    pts = {PietTree(QString("GameOver.png"))};
    ExecutingPietLibraries::functionHash[QString("GLLoadImage")](pts);
    int gameoverH = pts[0].Val();

    //pts = {PietTree(QString("NeverSayNever.mp3")),PietTree(20)};
    //ExecutingPietLibraries::functionHash[QString("GLPlayMusic")](pts);

    int meY = h / 2;
    int count = 0;
    int dead = 0;
    vector<pair<int,int>> birds;
    while(true){

        pts = {};
        ExecutingPietLibraries::functionHash[QString("GLProcessAll")](pts);
        if(pts[0].Val() == 0) break;

        if(!dead){
            count ++;

            pts = {PietTree(0),PietTree(0),PietTree(skyH)};
            ExecutingPietLibraries::functionHash[QString("GLDrawImage")](pts);

            pts = {PietTree(Qt::Key_Up)};
            ExecutingPietLibraries::functionHash[QString("GLGetKey")](pts);
            if(pts[0].Val())meY += 1;

            pts = {PietTree(Qt::Key_Down)};
            ExecutingPietLibraries::functionHash[QString("GLGetKey")](pts);
            if(pts[0].Val())meY -= 1;

            if(meY > h - size) meY = h - size;
            if(meY < 0) meY = 0 ;

            pts = {PietTree(meX),PietTree(meY),PietTree(meH)};
            ExecutingPietLibraries::functionHash[QString("GLDrawImage")](pts);

            if(count % birdemitcount == 0){
                pts = {PietTree(h - size)};
                ExecutingPietLibraries::functionHash[QString("random")](pts);
                birds.push_back(std::make_pair(w,pts[0].Val()));
            }
            for(pair<int,int>& bird : birds){
                bird.first -= birdspeed;

                pts = {PietTree(bird.first),PietTree(bird.second),PietTree(birdH)};
                ExecutingPietLibraries::functionHash[QString("GLDrawImage")](pts);

                if(bird.first + size < 0) {
                    pts = {PietTree(w)};
                    ExecutingPietLibraries::functionHash[QString("random")](pts);
                    bird = make_pair(w + pts[0].Val(),bird.second);
                }
                if(meX + size > bird.first && meX < bird.first + size && meY + size > bird.second && meY < bird.second + size ){
                    dead = 1;
                }
            }
            if(dead) {
                birds.clear();
                count = 0;
            }
        }else{
            count ++;
            pts = {PietTree(0),PietTree(0),PietTree(gameoverH)};
            ExecutingPietLibraries::functionHash[QString("GLDrawImage")](pts);
            if(count > 2000)dead = false;
        }
    }return 0;
}


int main(int argc, char *argv[]){
    ExecutingPietLibraries::Hash_FuncSet();
    PietCore::rootpath = (QFileInfo (argv[0])).absolutePath() + QDir::separator();
    QApplication a(argc, argv);

    if(argc >= 2 ){
        auto loadedimage = QImage(argv[1]);
        if(loadedimage.isNull() ){cout << "Invalid Image! " << endl; return 0;}

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
        } );

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


