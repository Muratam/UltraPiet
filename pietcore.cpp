#include "pietcore.h"
#include <iostream>

using namespace std;
const QRgb PietCore::normalColors[3][7] =  {
    { qRgb(255,192,192),qRgb(255,255,192),qRgb(192,255,192),qRgb(192,255,255),qRgb(192,192,255),qRgb(255,192,255),qRgb(255,255,255)},
    { qRgb(255,  0,  0),qRgb(255,255,  0),qRgb(  0,255,  0),qRgb(  0,255,255),qRgb(  0,  0,255),qRgb(255,  0,255),qRgb(  0,  0,  0)},
    { qRgb(192,  0,  0),qRgb(192,192,  0),qRgb(  0,192,  0),qRgb(  0,192,192),qRgb(  0,  0,192),qRgb(192,  0,192),qRgb(255,255,255)}
};

const QString PietCore::normalOrders[3][7] = {
    {QString("*"),   QString("add"),QString("div"),QString("great"), QString("dup"),  QString("in(c)") ,QString("")},
    {QString("push"),QString("sub"),QString("mod"),QString("point"), QString("roll"), QString("out(n)"),QString("")},
    {QString("pop"), QString("mul"),QString("not"),QString("switch"),QString("in(n)"),QString("out(c)"),QString("")}
};
const EOrder PietCore::normalEOrders[3][7] = {
    {EOrder::Same,EOrder::Add,EOrder::Div,EOrder::Great,EOrder::Dup,EOrder::InC,EOrder::Exception},
    {EOrder::Push,EOrder::Sub,EOrder::Mod,EOrder::Point,EOrder::Roll,EOrder::OutN,EOrder::Exception},
    {EOrder::Pop,EOrder::Mul,EOrder::Not,EOrder::Switch,EOrder::InN,EOrder::OutC,EOrder::Exception},
};
PietCore::PietCore() {
    stack = std::vector<int>();
    coded = std::vector<std::vector<int>>();
    init();
}
void PietCore::init(){
    dp=dpR;cc = ccL; w = h = 0;
    pos = QPoint(0,0);
    stack.clear();coded.clear();
}

QString PietCore::exec(){
    QString str ;
    REP(y,h){
        REP(x,w) str += QString("%1 ").arg(coded[x][y]);
        str += QString("\n");
    }
    return str;
}
void PietCore::setImage(const QImage & image){
    coded.clear();
    w = image.width();
    h = image.height();
    coded = vector<vector<int>>(image.width(), vector<int>(image.height()));
    REP(i,image.width()){
        REP(j,image.height()){
            REP(k,3) REP(l,7) if(image.pixel(i,j) == normalColors[k][l]){
                coded[i][j] = 3*l+k;
                goto COLORMATCHED;
            }
            qRgb(0,0,0);
            coded[i][j] = -1* image.pixel(i,j);
            COLORMATCHED:;
        }
    }
}
