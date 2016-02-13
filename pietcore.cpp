#include "pietcore.h"
#include <iostream>
#include <queue>
#include <unordered_set>
#include <QPoint>

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
    stack = vector<int>();
    coded = vector<vector<int>>();
    pos8 = vector<vector<Point8>>();
    init();
}
void PietCore::init(){
    dp=dpR;cc = ccL; w = h = 0;
    pos = QPoint(0,0);
    stack.clear();
    coded.clear();pos8.clear();
}

namespace std{
    template <>struct hash<QPoint>{
        size_t operator()(QPoint const & p) const noexcept{
            return p.x() > p.y() ?
                        p.x() * p.x() + p.y()         + 1:
                        p.y() * p.y() + p.x() + p.y() + 1;
        }
    };
}
#define PIXEL(POS) coded[ (POS) .x()][ (POS) .y()]
#define POINT8(POS) pos8[ (POS) .x()][ (POS) .y()]

void PietCore::search(QPoint me){//実行前にメモ化しておく
    if(POINT8(me).defined) return;
    const QPoint dR =QPoint(1,0),dD = QPoint(0,1),dL = QPoint(-1,0),dU = QPoint(0,-1);
    const QPoint dir[]{dR,dD,dL,dU};

    // 1Blockな可能性は高いので先に処理
    bool is1Block = true;
    auto c0 = PIXEL(me);
    for (auto& d : dir){
        if( isInImage(me+d) && c0 == PIXEL(me+d) ){
           is1Block = false;
        }
    }
    if(is1Block){
        POINT8(me) ={me+dR,me+dR,me+dD,me+dD,me+dL,me+dL,me+dU,me+dU,true};
        return;
    }

    queue<QPoint> bfs; // 次探索する場所
    unordered_set<QPoint> seeked; // 探索した場所を保存 (挿入O(1),存在確認O(1))
    Point8 res8 = {me,me,me,me,me,me,me,me,true};
    bfs.push(me);
    seeked.insert(me);
    while (bfs.size()>0){
        QPoint current = bfs.front();
        for (auto& d : dir){
            QPoint seek = current + d;

            if( isInImage(seek)) { //探索リストに追加
                if(c0 == PIXEL(seek) ){
                    if(seeked.find(seek) == seeked.end()){ //not found
                        bfs.push(seek);
                        seeked.insert(seek);
                    }
                }else { //res8に追加
                    #define CHECK8(DIR8,DPXY,LRXY,OP1,OP2) \
                        if( (DIR8.DPXY OP1 seek.DPXY) || \
                            (DIR8.DPXY == seek.DPXY && DIR8.LRXY OP2 seek.LRXY)) DIR8 = seek;
                    if (d==dR){
                        CHECK8(res8.RL,x(),y(),<,>);
                        CHECK8(res8.RR,x(),y(),<,<);
                    }else if(d==dD){
                        CHECK8(res8.DL,y(),x(),<,>);
                        CHECK8(res8.DR,y(),x(),<,<);
                    }else if(d==dL){
                        CHECK8(res8.LL,x(),y(),>,<);
                        CHECK8(res8.LR,x(),y(),>,>);
                    }else if(d==dU){
                        CHECK8(res8.UL,y(),x(),>,<);
                        CHECK8(res8.UR,y(),x(),>,>);
                    }
                }
            }else{
                if(d == dR)      res8.RL = res8.RR = Wall;
                else if(d == dD) res8.DL = res8.DR = Wall;
                else if(d == dL) res8.LL = res8.LR = Wall;
                else if(d == dU) res8.UL = res8.UR = Wall;
            }

        }
        bfs.pop();
    }
    for(const auto& s : seeked) POINT8(s) =res8;
}
void PietCore::setImage(const QImage & image){
    w = image.width();
    h = image.height();

    coded.clear();
    coded = vector<vector<int>>(image.width(), vector<int>(image.height()));
    REP(x,w){
        REP(y,h){
            REP(i,3) REP(j,7) if(image.pixel(x,y) == normalColors[i][j]){
                coded[x][y] = 3*j+i;
                goto COLORMATCHED;
            }
            coded[x][y] = -1* image.pixel(x,y);
            COLORMATCHED:;
        }
    }

    pos8.clear();
    pos8 =  vector<vector<Point8>>(image.width(), vector<Point8>(image.height()));
    REP(x,image.width()) REP(y,image.height()) search(QPoint(x,y));
}


QString PietCore::exec(){
    QString res ;
    auto c0 = PIXEL(pos);

    //REP(y,h)REP(x,w) res += QString("%1 ").arg(coded[x][y]);res += QString("\n");}
    return res;
}


