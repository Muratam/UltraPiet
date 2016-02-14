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
    dp=dpR;cc = ccL;processWallCount= step = w = h = 0;
    pos = QPoint(0,0);
    finished = false;
    Output = currentOrder = QString("");
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
        QPoint pR = me.x() == w - 1 ? Wall : me+dR;
        QPoint pD = me.y() == h - 1 ? Wall : me+dD;
        QPoint pL = me.x() == 0     ? Wall : me+dL;
        QPoint pU = me.y() == 0     ? Wall : me+dU;
        POINT8(me) ={pR,pR,pD,pD,pL,pL,pU,pU,true,1};
        return;
    }

    queue<QPoint> bfs; // 次探索する場所
    unordered_set<QPoint> seeked; // 探索した場所を保存 (挿入O(1),存在確認O(1))
    Point8 res8 = {me,me,me,me,me,me,me,me,true,1};
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
                        CHECK8(res8.DL,y(),x(),<,<);
                        CHECK8(res8.DR,y(),x(),<,>);
                    }else if(d==dL){
                        CHECK8(res8.LL,x(),y(),>,<);
                        CHECK8(res8.LR,x(),y(),>,>);
                    }else if(d==dU){
                        CHECK8(res8.UL,y(),x(),>,>);
                        CHECK8(res8.UR,y(),x(),>,<);
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
    res8.BlockSize = seeked.size();
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
EOrder PietCore::fromRelativeColor(int from,int to){
    // 0 3 6 9  12 15   18
    // 1 4 7 10 13 16   19
    // 2 5 8 11 14 17   20(将来のための)
    // 他は*=-1;
    if (from < 0 || to < 0) return EOrder::Exception; // 未実装
    if (from >= 18 || to >= 20 )return EOrder::Exception; //未実装
    if (to == 18) return EOrder::White;
    if (to == 19) return EOrder::Black;
    if(from % 3 != 0 && to % 3 == 0) to += 3;
    if(from % 3 == 2 && to % 3 == 1) to += 3;
    to -= (from % 3);
    from -= (from % 3);
    if (from > to) to += 18; // 上記の表に直す
    //enum class EOrder{Same=0,Push,Pop,Add,Sub,Mul,Div,Mod,Not,Great,Point,
    //                  Switch,Dup,Roll,InN,InC,OutN,OutC,White,Black,Exception};
    return (EOrder)(to -from);
}
void PietCore::processWall(){
    //Change CC,Change DP(時計回り)...
    if(processWallCount == 8) {
        processWallCount= 0;
        finished = true;
        return;
    }
    if(processWallCount % 2 == 0){
        cc = (cc == ccR ? ccL : ccR);
        currentOrder = QString("Change CC");
    }else{
        dp = (EDirectionPointer)((int)dp + 1);
        while(dp >= 4) dp = (EDirectionPointer)((int)dp - 4);
        currentOrder = QString("Change DP");
    }
    processWallCount ++;
}

void PietCore::execOneAction(){
    if (finished)return;
    step++;
    Point8 p8 = POINT8(pos);
    QPoint nextpos = p8.getbyDPCC(dp,cc);
    if(nextpos == Wall){processWall(); return ;}
    EOrder order = fromRelativeColor(PIXEL(pos),PIXEL(nextpos));
    int WhiteColor = 18;
    int BlackColor = 19;

    #define PROCESARITHMETICORDER(OPERATE) \
        if(stack.size() >= 2){ \
            int v1 = stack[stack.size()-1]; \
            int v2 = stack[stack.size()-2]; \
            stack.pop_back(); \
            stack.pop_back(); \
            stack.push_back(OPERATE); \
            currentOrder = QString(#OPERATE);\
        } \

    switch(order){
    case EOrder::Same: break; // 普通はないはず
    case EOrder::Push:
        stack.push_back(p8.BlockSize);
        currentOrder = QString("Push %1").arg(p8.BlockSize);
        break;
    case EOrder::Pop:
        if(stack.size() > 0)stack.pop_back();
        currentOrder = QString("Pop");
        break;
    case EOrder::Add:
        PROCESARITHMETICORDER(v2 + v1);
        break;
    case EOrder::Sub:
        PROCESARITHMETICORDER(v2 - v1);
        break;
    case EOrder::Mul:
        PROCESARITHMETICORDER(v2 * v1);
        break;
    case EOrder::Div:
        if(stack.size()>= 2 && stack[stack.size()-1] == 0) {break;} // Div 0
        PROCESARITHMETICORDER(v2 / v1);
        break;
    case EOrder::Mod:
        if(stack.size()>= 2 && stack[stack.size()-1] == 0) {break;} // Mod 0
        PROCESARITHMETICORDER(v2 % v1);
        break;
    case EOrder::Not:
        if(stack.size() > 0 ){
             stack[stack.size()-1] = stack[stack.size()-1] == 0 ? 1 : 0;
             currentOrder = QString("Not");
        }
        break;
    case EOrder::Great:
        PROCESARITHMETICORDER(v1 < v2 ? 0 : 1);
        break;
    case EOrder::Point:// 時計回り
        if(stack.size() > 0 ){ //-1 % 4 => -1
             dp = (EDirectionPointer)((int)dp + (stack[stack.size()-1] % 4));
             stack.pop_back();
             while (dp >= 4) { dp =(EDirectionPointer)((int)dp - 4); }
             currentOrder = QString("Point");
        }
        break;
    case EOrder::Switch:
        if(stack.size() > 0 ){ //-1 % 4 => -1
             switch(stack[stack.size()-1] % 2){
                case -1:case 1: cc = (cc == ccR ? ccL : ccR);
             }
             stack.pop_back();
             currentOrder = QString("Switch");
        }
        break;
    case EOrder::Dup:
        if(stack.size() > 0 ){
             stack.push_back( stack[stack.size()-1]);
             currentOrder = QString("Duplicate");
        }
        break;
    case EOrder::Roll:break; // 未実装
    case EOrder::InN:break; // 未実装
    case EOrder::InC:break; // 未実装
    case EOrder::OutN:
        if(stack.size() > 0){
            Output += QString("%1").arg(stack[stack.size()-1]);
            currentOrder = QString("Out %1").arg(stack[stack.size()-1]);
            stack.pop_back();
        }
        break;
    case EOrder::OutC:
        if(stack.size() > 0){
            Output += QString(stack[stack.size()-1]);
            currentOrder = QString("Out ")+ QString (stack[stack.size()-1]);
            stack.pop_back();
        }
        break;
    case EOrder::White:{
            QPoint searchpos = nextpos;
            for (;;searchpos += directionFromDP(dp)){
                if(!isInImage(searchpos) || PIXEL(searchpos) == BlackColor){processWall();return;}
                else if (PIXEL(searchpos) != WhiteColor){
                    nextpos = searchpos;
                    currentOrder = QString("Nop");
                    break;
                }
            }
        }break;
    case EOrder::Black: processWall();return;
    default: case EOrder::Exception:break;
    }
    pos = nextpos;
    processWallCount = 0;
}

//未実装(とりあえず10万Stepうちきり)
void PietCore::exec(){
    while (finished || step > 100000){execOneAction();}
}

QString PietCore::printStack(){
    auto res = QString ("");
    for(auto& s : stack){
        res += QString("%1 (").arg(s) + QString(s) + QString(")\n");
    }
    return res;
}

QString PietCore::printStatus(){
    QString res("");
    res += QString("w:%1 * h:%2\n").arg(w).arg(h);
    res += QString("x:%1 , y:%2\n").arg(pos.x()).arg(pos.y());
    //res += normalOrders[][];
    res += QString("DP:%1(%2)\n").arg(arrowFromDP(dp)).arg((int)dp);
    res += QString("CC:%1(%2)\n").arg(cc == ccR ? "R" : "L").arg((int)cc);
    res += QString("Step:%1\n").arg(step);
    res += currentOrder + QString("\n");
    //res += QString("CS:1\n");//codelsize
    return res;
}
