#include "mainwindow.h"
#include <QApplication>
#include <QImage>
#include <QFile>
#include <QString>
#include <QTextStream>
#include "pietcore.h"
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


auto MakeGLView = [=](QWidget *parent ,int w ,int h,QString title){
    GLGameWidget* glgw = GLGameWidget::MakeUniqueGLWidget(parent);
    if(glgw == nullptr) return ;
    glgw->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    glgw->setSize(w,h);

    QDockWidget* dw = new QDockWidget(parent);
    dw->setFloating(true);
    dw->setAllowedAreas(Qt::NoDockWidgetArea);
    dw->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    dw->setWidget(glgw);
    dw->setAttribute(Qt::WA_DeleteOnClose);
    dw->setWindowTitle(title);
    dw->show();
};


auto GLProcessAll = [=](){
    QApplication::processEvents();
    Sleep(1);
    if(GLGameWidget::getUniqueGLWidget() == nullptr) return false;
    GLGameWidget::getUniqueGLWidget()->updateGL();
    GLGameWidget::getUniqueGLWidget()->refresh();
    return true;
};

auto GLDrawRect = [=](int x ,int y,int w,int h,QColor color){
    if(GLGameWidget::getUniqueGLWidget() == nullptr) return false;
    else GLGameWidget::getUniqueGLWidget()->drawRect(x,y,w,h,color);
    return true;
};


int main(int argc, char *argv[]){
    ExecutingPietLibraries::Hash_FuncSet();
    PietCore::rootpath = (QFileInfo (argv[0])).absolutePath() + QDir::separator();

//#define DEBUGSHOWOPENGL
#ifdef DEBUGSHOWOPENGL
    QApplication a(argc, argv);
    MakeGLView(NULL,600,400,QString("Music Game"));
    int x = 0, y = 0;
    while(GLProcessAll()){
        x++;y++;if(x > 300) x = 0; if(y > 300) y = 0;
        GLDrawRect(x,y,200,200,QColor(255,255,0));
        GLDrawRect(600- x,400 - y,200,200,QColor(0,255,0));
    }return 0;
#endif
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


