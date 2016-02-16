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
    {QString("*"),   QString("add\nappend") ,QString("div\nmatch"),QString("greater\nfile")  , QString("dup")      , QString("in(c)")       ,QString("")},
    {QString("push"),QString("sub\nsplit")  ,QString("mod\nzip")  ,QString("point\nhead")    , QString("roll\ndll"), QString("out(n)\nsize"),QString("")},
    {QString("pop"), QString("mul\nproduct"),QString("not")       ,QString("switch\nflatten"), QString("in(n)")    , QString("out(c)")      ,QString("")}
};
const EOrder PietCore::normalEOrders[3][7] = {
    {EOrder::Same,EOrder::Add,EOrder::Div,EOrder::Great,EOrder::Dup,EOrder::InC,EOrder::Exception},
    {EOrder::Push,EOrder::Sub,EOrder::Mod,EOrder::Point,EOrder::Roll,EOrder::OutN,EOrder::Exception},
    {EOrder::Pop,EOrder::Mul,EOrder::Not,EOrder::Switch,EOrder::InN,EOrder::OutC,EOrder::Exception},
};

PietCore::PietCore() {
    stack = vector<PietTree>();
    coded = vector<vector<int>>();
    pos8 = vector<vector<Point8>>();
    init([](QString qs){},[](){return QChar(72);});
}
void PietCore::init(function<void(QString)> outPutFunction,function<QChar(void)>inPutCharFunction){
    dp=dpR;cc = ccL;processWallCount= step = w = h = 0;
    pos = QPoint(0,0);
    finished = false;
    LightcurrentOrder = currentOrder = QString("");
    stack.clear();
    coded.clear();pos8.clear();
    this->outPutFunction = outPutFunction;
    this->inPutCharFunction = inPutCharFunction;
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
        LightcurrentOrder =  QString("");
    }else{
        dp = (EDirectionPointer)((int)dp + 1);
        while(dp >= 4) dp = (EDirectionPointer)((int)dp - 4);
        currentOrder = QString("Change DP");
        LightcurrentOrder = QString("");
    }
    processWallCount ++;
}

void PietCore::execOneAction(){
    if (finished)return;
    Point8 p8 = POINT8(pos);
    QPoint nextpos = p8.getbyDPCC(dp,cc);
    if(nextpos == Wall){processWall(); return ;}
    EOrder order = fromRelativeColor(PIXEL(pos),PIXEL(nextpos));
    int WhiteColor = 18;
    int BlackColor = 19;
    #define STACK_TOP stack[stack.size()-1]
    #define PROCESARITHMETICORDER(OPERATE,OPERATENAME,LIGHTOPERATE) \
        if(stack.size() >= 2){ \
            auto v1 = STACK_TOP; \
            stack.pop_back(); \
            auto v2 = STACK_TOP; \
            stack.pop_back(); \
            OPERATE;\
            stack.push_back(v2); \
            currentOrder = QString(OPERATENAME);\
            LightcurrentOrder = QString(LIGHTOPERATE);\
        }else { \
            currentOrder = QString("Less Arg");\
            LightcurrentOrder = QString ("！");\
        }
    #define SET_CURRENT_ORDER_LESS_ARGS {currentOrder = QString("Less Arg");LightcurrentOrder = QString("！");}
    switch(order){
    case EOrder::Same: break; // 普通はないはず
    case EOrder::Push:
        stack.push_back(PietTree(p8.BlockSize));
        currentOrder = QString("Push %1").arg(p8.BlockSize);
        LightcurrentOrder = p8.BlockSize < 100 ? QString::number(p8.BlockSize) : QString(p8.BlockSize);
        break;
    case EOrder::Pop:
        if(stack.size() > 0)stack.pop_back();
        currentOrder = QString("Pop");
        LightcurrentOrder = QString("♪");
        break;
    case EOrder::Add:
        PROCESARITHMETICORDER(v2.append(v1),"Add","＋");
        break;
    case EOrder::Sub:
        PROCESARITHMETICORDER(v2.split (v1),"Sub","-");
        break;
    case EOrder::Mul:
        PROCESARITHMETICORDER(v2.product(v1),"Mul","×");
        break;
    case EOrder::Div:
        if(stack.size()< 2){ SET_CURRENT_ORDER_LESS_ARGS; break;}
        if(STACK_TOP .isLeaf() && STACK_TOP.Val() == 0){
            break;
        } // Div 0 => Read Module
        PROCESARITHMETICORDER(v2.match (v1),"Div","÷");
        break;
    case EOrder::Mod:
        if(stack.size() < 2){SET_CURRENT_ORDER_LESS_ARGS; break;}
        if(STACK_TOP .isLeaf() && STACK_TOP.Val() == 0){
            stack.pop_back();
            int n = (STACK_TOP.isLeaf() ? STACK_TOP.Val(): INT_MAX - 1 );
            stack.pop_back();
            PietTree::MakeStackByMod0(n,stack);
            currentOrder = QString("ToStack");
            LightcurrentOrder = QString("%0");
            break;
        } // Mod 0 => MakeStack
        PROCESARITHMETICORDER(v2.zip (v1),"Mod","％");
        break;
    case EOrder::Not:
        if(stack.size() > 0 ){
             STACK_TOP = PietTree( STACK_TOP .Not()) ;
             currentOrder = QString("Not");
             LightcurrentOrder = QString("0?");
        }else{SET_CURRENT_ORDER_LESS_ARGS}
        break;
    case EOrder::Great: //v1 < v2 ? 0 : 1
        PROCESARITHMETICORDER(v2.loadFile(v1),"Greater","＞");
        break;
    case EOrder::Point:
        if(stack.size() > 0 ){
            if(! STACK_TOP.isLeaf()){
                stack.push_back(STACK_TOP.popHead());
                currentOrder = QString("head");
                LightcurrentOrder = QString("HD");
            }else{// 時計回り -1 % 4 => -1
                dp = (EDirectionPointer)((int)dp + (STACK_TOP.Val() % 4));
                stack.pop_back();
                while (dp >= 4) { dp =(EDirectionPointer)((int)dp - 4); }
                currentOrder = QString("Point");
                LightcurrentOrder = arrowFromDP(dp);
            }
        }else {SET_CURRENT_ORDER_LESS_ARGS}
        break;
    case EOrder::Switch:
        if(stack.size() > 0 ){ //-1 % 4 => -1
            if(! STACK_TOP.isLeaf()){
                STACK_TOP.flatten();
                currentOrder = QString("flatten");
                LightcurrentOrder = QString("⊥");
            }else{
                if(STACK_TOP.Val() % 2 != 0)
                    cc = (cc == ccR ? ccL : ccR);
                stack.pop_back();
                currentOrder = QString("Switch");
                LightcurrentOrder = QString(cc == ccR ? "cR" :"cL" );
            }
        }else {SET_CURRENT_ORDER_LESS_ARGS}
        break;
    case EOrder::Dup:
        if(stack.size() > 0 ){
             stack.push_back( STACK_TOP);
             currentOrder = QString("Duplicate");
             LightcurrentOrder = QString("x2");
        }else {SET_CURRENT_ORDER_LESS_ARGS}
        break;
    case EOrder::Roll:
        if(stack.size() > 2){
            if(!stack[stack.size()-1].isLeaf() || !stack[stack.size()-2].isLeaf()){
                //未実装ーーーーーー
                break;
            }
            // [2,3,4,3,2,1] => 2,3とPopして,深さ3まで2回転 : [4,3,2,1] => [3,2,4,1] => [2,4,3,1]//PidetのRollはO(depth*roll)だった
            int roll = stack[stack.size()-1].Val();
            int depth = stack[stack.size()-2].Val();
            int forCurrentOrderRoll = roll;
            if(depth < 0) break;//負Rollは無視
            if(stack.size() - 2 < depth) break;//depthの方が大きい場合も無視
            stack.pop_back();
            stack.pop_back();
            if(stack.size()==0) break;
            roll = roll % depth ;      // Pidetはここ微妙に何故か違った
            vector<PietTree> copy;
            REP(i,depth) copy.push_back( stack[stack.size() - depth + i ]);
            REP(i,depth) stack[stack.size() - depth + i] =
                            i - roll < 0 ? copy[i - roll + depth ] : copy[i - roll] ;
            currentOrder = QString("Roll d %1,r %2").arg(depth).arg(forCurrentOrderRoll);
            LightcurrentOrder = QString("Rl");
        }else{SET_CURRENT_ORDER_LESS_ARGS}
        break;
    case EOrder::InN:break; // 未実装Pidetでは12,13,14のようにセパレータを出来ないので、/[0-9]+/を取る
    case EOrder::InC:{
        QChar gotChar = inPutCharFunction();
        stack.push_back(PietTree(gotChar));
        currentOrder = QString("In ") + QString(gotChar);
        LightcurrentOrder =  QString(gotChar);
        } break;
    case EOrder::OutN: //size
        if(stack.size() > 0){
            if(! STACK_TOP.isLeaf()){
                PietTree top = STACK_TOP;
                int size = top.Nodes().size();
                stack.pop_back();
                for(auto t : top.Nodes()) stack.push_back(t);
                stack.push_back(PietTree(size));
            }else{
                outPutFunction( QString("%1").arg(STACK_TOP.Val()));
                currentOrder = QString("Out %1").arg(STACK_TOP.Val());
                LightcurrentOrder =(STACK_TOP.Val() >= 0 && STACK_TOP.Val() < 100)?QString::number(STACK_TOP.Val()):QString("ON");
                stack.pop_back();
            }
        }else{SET_CURRENT_ORDER_LESS_ARGS}
        break;
    case EOrder::OutC:
        if(stack.size() > 0){
            QString str = STACK_TOP.toString();
            outPutFunction(str);
            currentOrder = QString("Out ") + (STACK_TOP.isLeaf() ? str : QString("(C)"));
            LightcurrentOrder = (STACK_TOP.isLeaf() ? str : QString("OC"));
            stack.pop_back();
        }else{SET_CURRENT_ORDER_LESS_ARGS}
        break;
    case EOrder::White:{
            QPoint searchpos = nextpos;
            for (;;searchpos += directionFromDP(dp)){
                if(!isInImage(searchpos) || PIXEL(searchpos) == BlackColor){processWall();return;}
                else if (PIXEL(searchpos) != WhiteColor){
                    nextpos = searchpos;
                    currentOrder = QString("Nop");
                    LightcurrentOrder =  QString("");
                    break;
                }
            }
        }break;
    case EOrder::Black: processWall();return;
    default: case EOrder::Exception:break;
    }
    step++;
    pos = nextpos;
    processWallCount = 0;
}

QString PietCore::printStack(){
    auto res = QString ("");
    for(auto& s : stack){
        //res += QString("%1 (").arg(s) + QString(s) + QString(")\n");
        res += s.showStack(showStackAsNumber) +QString("\n");
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
/*
QChar PietCore::getInputQChar(){
    if(Input.isEmpty() || Input.isNull()) throw false;
    QChar res = Input.at(0);
    Input.remove(0,1);
    return res;
}
int PietCore::getInputNumber()throw (bool){
    if(Input.isEmpty() || Input.isNull()) throw false;
    QRegExp rx("^-?\\d+");
    if(-1 == rx.indexIn(Input)) throw true ;
    QString Match = rx.cap();
    int m = Match.toInt();
    Input.remove(0,Match.length());
    return m;
}
*/
