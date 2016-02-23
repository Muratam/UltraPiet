#include "executingpietlibraries.h"
#include "piettree.h"

#include <QtWidgets>
#include <glgamewidget.h>
#include <random>

QHash<QString,std::function<void(std::vector<PietTree>&)>> ExecutingPietLibraries::functionHash;
bool ExecutingPietLibraries::Hash_HadSet = false;
void ExecutingPietLibraries::Hash_FuncSet(){
    //C++で定義した関数はここで関数ハッシュに登録すると使える
    //二回関数名と呼び出し名で分けて書いてしまうと整合性が取れない可能性があるため文字列化のマクロを用いている
    #define REGISTFUNCTION(FUNCTIONNAME) functionHash[QString( #FUNCTIONNAME)] = (std::function<void(std::vector<PietTree>&)>) FUNCTIONNAME;
    if(Hash_HadSet) return;
    Hash_HadSet = true;
    REGISTFUNCTION(random);
    REGISTFUNCTION(MakeGLView);
    REGISTFUNCTION(GLProcessAll);
    REGISTFUNCTION(GLDrawRect);
    REGISTFUNCTION(GLLoadImage);
    REGISTFUNCTION(GLDrawImage);
    REGISTFUNCTION(GLGetKey);
    REGISTFUNCTION(GLPlayMusic);
    #undef REGISTFUNCTION
}

std::mt19937 rnd ;
void ExecutingPietLibraries::random(std::vector<PietTree> & pt){
    if(pt.size() < 1) return;
    int max = pt[0].Val();
    for(int i:range(1)) pt.pop_back();
    std::uniform_int_distribution<int> randomrange( 0, max ) ;
    pt.push_back(PietTree( abs(randomrange(rnd))));
    return;
}


void ExecutingPietLibraries::MakeGLView (std::vector<PietTree> & pt){
    if(pt.size() < 3) return;
    int w = pt[0].Val();
    int h = pt[1].Val();
    QString title = pt[2].toString();
    for(int i:range(3)) pt.pop_back();
    GLGameWidget* glgw = GLGameWidget::MakeUniqueGLWidget(nullptr);
    if(glgw == nullptr) return ;
    glgw->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    glgw->setSize(w,h);

    QDockWidget* dw = new QDockWidget(nullptr);
    dw->setFloating(true);
    dw->setAllowedAreas(Qt::NoDockWidgetArea);
    dw->connect(dw,QDockWidget::dockLocationChanged,[=](){ dw->setFloating(true);});
    dw->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    dw->setWidget(glgw);
    dw->setAttribute(Qt::WA_DeleteOnClose);
    dw->setWindowTitle(title);
    dw->show();
}

void ExecutingPietLibraries::GLProcessAll (std::vector<PietTree> & pt){
    QApplication::processEvents();
    Sleep(1);
    if(GLGameWidget::getUniqueGLWidget() == nullptr){pt.push_back(PietTree(0)); return ;}
    GLGameWidget::getUniqueGLWidget()->updateGL();
    GLGameWidget::getUniqueGLWidget()->refresh();
    pt.push_back(PietTree(1));
}

void ExecutingPietLibraries::GLDrawRect(std::vector<PietTree> & pt){
    if(pt.size() < 7) return;
    int x = pt[0].Val();
    int y = pt[1].Val();
    int w = pt[2].Val();
    int h = pt[3].Val();
    int r = pt[4].Val();
    int g = pt[5].Val();
    int b = pt[6].Val();
    for(int i:range(7)) pt.pop_back();
    if(GLGameWidget::getUniqueGLWidget() != nullptr)
       GLGameWidget::getUniqueGLWidget()->drawRect(x,y,w,h,QColor(r,g,b));
}

void ExecutingPietLibraries::GLLoadImage(std::vector<PietTree> & pt){
    if(pt.size() < 1) return;
    QString path = pt[0].toString();
    for(int i:range(1)) pt.pop_back();
    if(GLGameWidget::getUniqueGLWidget() != nullptr){
       pt.push_back(PietTree(GLGameWidget::getUniqueGLWidget()->loadImage(path)));
    }else {
        pt.push_back(PietTree(0));
    }
}

void ExecutingPietLibraries::GLDrawImage(std::vector<PietTree> & pt){
    if(pt.size() < 3) return;
    int x = pt[0].Val();
    int y = pt[1].Val();
    int handle = pt[2].Val();
    for(int i:range(3)) pt.pop_back();
    if(GLGameWidget::getUniqueGLWidget() != nullptr)
       GLGameWidget::getUniqueGLWidget()->drawImage(x,y,handle);
}

void ExecutingPietLibraries::GLGetKey(std::vector<PietTree> & pt){
    if(pt.size() < 1) return;
    int keyCode = pt[0].Val();
    for(int i:range(1)) pt.pop_back();
    GLGameWidget * gw = GLGameWidget::getUniqueGLWidget();
    if(gw == nullptr){pt.push_back(PietTree(0)); return;}
    else{pt.push_back(PietTree( gw->getKeyDown(keyCode)));return;}
}

void ExecutingPietLibraries::GLPlayMusic(std::vector<PietTree> & pt){
    if(pt.size() < 2) return;
    QString path = pt[0].toString();
    int Volume = pt[1].Val();
    for(int i : range(2)) pt.pop_back();
    GLGameWidget * gw = GLGameWidget::getUniqueGLWidget();
    if(gw == nullptr) return;
    gw->mp.setMedia(QUrl::fromLocalFile(path));
    gw->mp.setVolume(Volume);
    gw->mp.play();
}
