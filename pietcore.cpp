#include "pietcore.h"
#include "executingpietlibraries.h"
#include "loadpietdll.h"
#include <iostream>
#include <queue>
#include <string>
#include <unordered_set>
#include <QPoint>
#include <QDir>
using namespace std;

QString PietCore::rootpath ;


const QRgb PietCore::normalColors[3][7] =  {
    { qRgb(255,192,192),qRgb(255,255,192),qRgb(192,255,192),qRgb(192,255,255),qRgb(192,192,255),qRgb(255,192,255),qRgb(255,255,255)},
    { qRgb(255,  0,  0),qRgb(255,255,  0),qRgb(  0,255,  0),qRgb(  0,255,255),qRgb(  0,  0,255),qRgb(255,  0,255),qRgb(  0,  0,  0)},
    { qRgb(192,  0,  0),qRgb(192,192,  0),qRgb(  0,192,  0),qRgb(  0,192,192),qRgb(  0,  0,192),qRgb(192,  0,192),qRgb(255,255,255)}
};
const QString PietCore::normalOrders[3][7] = {
    {QString("* \nU tf-8"),   QString("+ add\nappend") ,QString("/ div\nmatch"),QString("<great\nfile")    , QString("Ｄ up")      , QString("ｉ n(c)")       ,QString("SPACE")},
    {QString("p ush"),QString("- sub\nsplit")  ,QString("% mod\nzip")  ,QString("@point\nlast") , QString("Ｒ oll\ndll"), QString("Ｏ ut(n)\nnodes"),QString("Ｂ lack")},
    {QString("P op♪"), QString("* mul\nprod")   ,QString("! not")   ,QString("^switch\nflat")   , QString("Ｉ n(n)")    , QString("ｏ ut(c)")      ,QString("Ｕ ser\n insert")}
};
const EOrder PietCore::normalEOrders[3][7] = {
    {EOrder::Same,EOrder::Add,EOrder::Div,EOrder::Great,EOrder::Dup,EOrder::InC,EOrder::Exception},
    {EOrder::Push,EOrder::Sub,EOrder::Mod,EOrder::Point,EOrder::Roll,EOrder::OutN,EOrder::Exception},
    {EOrder::Pop,EOrder::Mul,EOrder::Not,EOrder::Switch,EOrder::InN,EOrder::OutC,EOrder::Exception},
};

PietCore::PietCore( function<void(QString)> outPutFunction, function<QChar(void)>inPutCharFunction , function<int(bool &)> inPutNumFunction){
    stack = vector<PietTree>();
    coded = vector<vector<int>>();
    pos8 = vector<vector<Point8>>();
    init(outPutFunction,inPutCharFunction,inPutNumFunction);
}
void PietCore::init(function<void(QString)> outPutFunction, function<QChar(void)>inPutCharFunction , function<int(bool &)> inPutNumFunction){
    dp=dpR;cc = ccL;processWallCount= step = w = h = 0;
    pos = QPoint(0,0);
    finished = false;
    LightcurrentOrder = currentOrder = QString("");
    stack.clear();
    coded.clear();pos8.clear();
    this->outPutFunction = outPutFunction;
    this->inPutCharFunction = inPutCharFunction;
    this->inPutNumFunction = inPutNumFunction;
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

void PietCore::search(QPoint me){//実行前にメモ化しておく
    if(Pos8(me).defined) return;
    const QPoint dR =QPoint(1,0),dD = QPoint(0,1),dL = QPoint(-1,0),dU = QPoint(0,-1);
    const QPoint dir[]{dR,dD,dL,dU};

    // 1Blockな可能性は高いので先に処理
    bool is1Block = true;
    auto c0 = Coded(me);
    for (auto& d : dir){
        if( isInImage(me+d) && c0 == Coded(me+d) ){
           is1Block = false;
        }
    }
    if(is1Block){
        QPoint pR = me.x() == w - 1 ? Wall : me+dR;
        QPoint pD = me.y() == h - 1 ? Wall : me+dD;
        QPoint pL = me.x() == 0     ? Wall : me+dL;
        QPoint pU = me.y() == 0     ? Wall : me+dU;
        pos8[ me.x()][ me.y()] ={pR,pR,pD,pD,pL,pL,pU,pU,true,1};
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
                if(c0 == Coded(seek) ){
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
                    #undef CHECK8
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
    for(const auto& s : seeked) pos8[s.x()][s.y()] =res8;
}
void PietCore::setImage(const QImage & image,QString ImagePath){
    this->ImagePath =  ImagePath;
    w = image.width();
    h = image.height();

    coded.clear();
    coded = vector<vector<int>>(image.width(), vector<int>(image.height()));
    for(auto x : range(w)){
        for(auto y : range(h)){
            for(auto i : range(3)) for (auto j:range(7)) if(image.pixel(x,y) == normalColors[i][j]){
                coded[x][y] = 3*j+i;
                goto COLORMATCHED;
            }
            coded[x][y] = -1* image.pixel(x,y);
            COLORMATCHED:;
        }
    }
    //if(coded[0][0] < 0 || coded[0][0] >= 18 ) coded[0][0] = normalColors[0][0]; //普通のPietでは無い場合

    pos8.clear();
    pos8 =  vector<vector<Point8>>(image.width(), vector<Point8>(image.height()));
    for(auto x: range(image.width())) for(auto y:range(image.height())) search(QPoint(x,y));
}
EOrder PietCore::fromRelativeColor(int from,int to){
    if (from < 0 || to < 0) return EOrder::Exception; // 未実装
    if (from >= 18 || to >= 20 )return EOrder::Exception; //未実装
    if (to == 18) return EOrder::White;
    if (to == 19) return EOrder::Black;
    if(from % 3 != 0 && to % 3 == 0) to += 3;
    if(from % 3 == 2 && to % 3 == 1) to += 3;
    to -= (from % 3);
    from -= (from % 3);
    if (from > to) to += 18; // 下記の表に直す
    return (EOrder)(to -from);
}
//enum class EOrder{Same=0,Push,Pop,Add,Sub,Mul,Div,Mod,Not,Great,Point,
//                  Switch,Dup,Roll,InN,InC,OutN,OutC,White,Black,Exception};
// 0 3 6 9  12 15   18
// 1 4 7 10 13 16   19
// 2 5 8 11 14 17   20(将来のための)// 他は*=-1;

QRgb PietCore::getNormalColor(int nowCode,EOrder nextOrder){
    if     (nextOrder == EOrder::White) return normalColors[0][6];
    else if(nextOrder == EOrder::Black) return normalColors[1][6];
    if (nowCode < 0 || nowCode >= 20) return normalColors[0][0]; //Error
    if (nowCode == 18 || nowCode == 19) return normalColors[0][0]; //とりあえず基準のあの色を返しておく
    int nextOrderCode = (int)nextOrder;
    QPoint nextOrderPos = QPoint (nowCode / 3,nowCode % 3) + QPoint (nextOrderCode / 3  ,nextOrderCode % 3);;
    return normalColors[nextOrderPos.y() % 3][nextOrderPos.x() % 6];
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
        setCurrentOrderLessArgs(); \
    }
void PietCore::execOneAction(){
    if (finished)return;
    Point8 p8 = Pos8(pos);
    QPoint nextpos = p8.getbyDPCC(dp,cc);
    const int WhiteColor = 18;
    const int BlackColor = 19;
    if(nextpos == Wall){processWall(); return ;}
    int currentcode = Coded(pos);
    int nextcode = Coded(nextpos);
    if(currentcode < 0 || currentcode > BlackColor) { //Unicode
        if(nextcode < 0 || nextcode > BlackColor) currentcode = 0; //unicodeが連続
        else if (nextcode == BlackColor) currentcode = 0; //壁
        else currentcode = nextcode ; //Unicode => NormalColor はなにもしない
    }else  if(currentcode == WhiteColor && nextcode == BlackColor) currentcode = 0; //壁
    EOrder order = fromRelativeColor(currentcode,nextcode);
    auto setCurrentOrderLessArgs= [this] (){currentOrder = QString("Less Arg");LightcurrentOrder = QString("??");};
    switch(order){
    case EOrder::Same:
        currentOrder = QString("Nop");
        LightcurrentOrder =  QString("Nop");
        break; //Unicode => NormalColor
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
    case EOrder::Div:{
        if(stack.size()< 2){ setCurrentOrderLessArgs(); break;}
        if(STACK_TOP .isLeaf() && STACK_TOP.Val() == 0){
            PietTree cp = PietTree(stack[stack.size() - 2]);
            QString readpath = ( cp.isLeaf()?QString::number(cp.Val()) : cp.toString() );
            if(readpath.size() > 2 && readpath[0].unicode() == 'L' && (readpath[1].unicode() == '/' || readpath[1].unicode() == '\\'  ) ){
                readpath = readpath.mid(2);
                if( doesCPPPietLibExist(readpath)){
                    ExecLib(readpath);
                    stack.pop_back(); stack.pop_back();
                    currentOrder = QString("Load LIB") ;
                    LightcurrentOrder = QString("÷0");
                }else {
                    currentOrder = QString("Load Miss") ;
                    LightcurrentOrder = QString("??");
                }
            }else{
                readpath += QString(".png");
                if(readpath.size() > 2 && readpath[0].unicode() == 'G' && (readpath[1].unicode() == '/' || readpath[1].unicode() == '\\'  ) ){
                    readpath = rootpath + readpath;//GlobalPietCode
                }
                QImage loadedimage = QImage(readpath);
                if(loadedimage.isNull()){
                    currentOrder = QString("Load Miss") ;
                    LightcurrentOrder = QString("??");
                }else{
                    stack.pop_back(); stack.pop_back();
                    currentOrder = QString("Load ") + readpath;
                    LightcurrentOrder = QString("÷0");
                    ExecOtherPietCore(loadedimage,readpath);
                }
            }
            break;
        }else{ // Div 0 => Read Module
            PROCESARITHMETICORDER(v2.match (v1),"Div","÷");
        }}break;
    case EOrder::Mod:
        if(stack.size() < 2){setCurrentOrderLessArgs(); break;}
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
             LightcurrentOrder = QString("!");
        }else{setCurrentOrderLessArgs();}
        break;
    case EOrder::Great: //v1 < v2 ? 0 : 1
        PROCESARITHMETICORDER(v2.loadFile(v1),"Greater","＞");
        break;
    case EOrder::Point:
        if(stack.size() > 0 ){
            if(! STACK_TOP.isLeaf()){
                stack.push_back(STACK_TOP.popHead());
                currentOrder = QString("last");
                LightcurrentOrder = QString("＠");
            }else{// 時計回り -1 % 4 => -1
                dp = (EDirectionPointer)((int)dp + (STACK_TOP.Val() % 4));
                stack.pop_back();
                while (dp >= 4) { dp =(EDirectionPointer)((int)dp - 4); }
                currentOrder = QString("Point");
                LightcurrentOrder = arrowFromDP(dp);
            }
        }else {setCurrentOrderLessArgs();}
        break;
    case EOrder::Switch:
        if(stack.size() > 0 ){ //-1 % 4 => -1
            if(! STACK_TOP.isLeaf()){
                STACK_TOP.flatten();
                currentOrder = QString("flatten");
                LightcurrentOrder = QString("＾");
            }else{
                if(STACK_TOP.Val() % 2 != 0)
                    cc = (cc == ccR ? ccL : ccR);
                stack.pop_back();
                currentOrder = QString("Switch");
                LightcurrentOrder = QString(cc == ccR ? "cR" :"cL" );
            }
        }else {setCurrentOrderLessArgs();}
        break;
    case EOrder::Dup:
        if(stack.size() > 0 ){
             stack.push_back( STACK_TOP);
             currentOrder = QString("Duplicate");
             LightcurrentOrder = QString("Ｄ");
        }else {setCurrentOrderLessArgs();}
        break;
    case EOrder::Roll: //DLL
        if(stack.size() > 2){
            if(!stack[stack.size()-1].isLeaf() || !stack[stack.size()-2].isLeaf()){
                if(stack.size() < 4) {setCurrentOrderLessArgs(); break;}
                PietTree ppath = stack[stack.size() - 4]; QString spath = ppath.toString();
                PietTree pfunc = stack[stack.size() - 3]; QString sfunc = pfunc.toString();
                PietTree ptype = stack[stack.size() - 2]; QString stype = ptype.toString();
                vector<PietTree> pts = stack[stack.size()-1].Nodes();
                bool Miss = false;
                PietTree res = LoadPietDLL::LoadDLL(Miss,spath,sfunc,stype,pts);
                for(auto i:range(4))stack.pop_back();
                if( Miss ){
                    currentOrder = QString("Dll Miss");
                    LightcurrentOrder = QString("??");
                    break;
                }else{
                    stack.push_back(pts);
                    stack.push_back(res);
                    currentOrder = QString("Dll Load");
                    LightcurrentOrder = QString("DL");
                }
                break;
            }
            // [2,3,4,3,2,1] => 2,3とPopして,深さ3まで2回転 : [4,3,2,1] => [3,2,4,1] => [2,4,3,1]//PidetのRollはO(depth*roll)だった
            int roll = stack[stack.size()-1].Val();
            int depth = stack[stack.size()-2].Val();
            int forCurrentOrderRoll = roll;
            if(depth <= 0) break;//負Rollは無視
            if(stack.size() - 2 < depth) break;//depthの方が大きい場合も無視
            stack.pop_back();
            stack.pop_back();
            if(stack.size()==0) break;
            roll = roll % depth ;      // Pidetはここ微妙に何故か違った
            vector<PietTree> copy;
            for(auto i:range(depth)) copy.push_back( stack[stack.size() - depth + i ]);
            for(auto i:range(depth)) stack[stack.size() - depth + i] =
                            i - roll < 0 ? copy[i - roll + depth ] : copy[i - roll] ;
            currentOrder = QString("Roll d %1,r %2").arg(depth).arg(forCurrentOrderRoll);
            LightcurrentOrder = QString("Ｒ");
        }else{setCurrentOrderLessArgs();}
        break;
    case EOrder::InN:{
        bool Missed = false;
        int gotNum = inPutNumFunction(Missed);
        if(Missed){
            currentOrder = QString("In(Num) Miss !");
            LightcurrentOrder = QString ("??");
        }else{
            stack.push_back(PietTree(gotNum));
            currentOrder = QString("In ") + QString::number(gotNum);
            LightcurrentOrder =(-10 < gotNum && gotNum <100)?  QString::number(gotNum):QString(gotNum);
        }}break;
    case EOrder::InC:{
        QChar gotChar = inPutCharFunction();
        stack.push_back(PietTree(gotChar));
        currentOrder = QString("In ") + QString(gotChar);
        LightcurrentOrder =  QString(gotChar);
        } break;
    case EOrder::OutN: //Nodes
        if(stack.size() > 0){
            if(! STACK_TOP.isLeaf()){
                PietTree top = STACK_TOP;
                int size = top.Nodes().size();
                stack.pop_back();
                for(auto t : top.Nodes()) stack.push_back(t);
                stack.push_back(PietTree(size));
                currentOrder = QString("Nodes");
                LightcurrentOrder = QString("NO");
            }else{
                outPutFunction( QString("%1").arg(STACK_TOP.Val()));
                currentOrder = QString("Out %1").arg(STACK_TOP.Val());
                LightcurrentOrder =(STACK_TOP.Val() >= 0 && STACK_TOP.Val() < 100)?QString::number(STACK_TOP.Val()):QString("ON");
                stack.pop_back();
            }
        }else {setCurrentOrderLessArgs();}
        break;
    case EOrder::OutC:
        if(stack.size() > 0){
            QString str = STACK_TOP.toString();
            outPutFunction(str);
            currentOrder = QString("Out ") + (STACK_TOP.isLeaf() ? str : QString("(C)"));
            LightcurrentOrder = (STACK_TOP.isLeaf() ? str : QString("OC"));
            stack.pop_back();
        }else{setCurrentOrderLessArgs();}
        break;
    case EOrder::White:{
            QPoint searchpos = nextpos;
            for (;;searchpos += directionFromDP(dp)){
                if(!isInImage(searchpos) || Coded(searchpos) == BlackColor){processWall();return;}
                else if (Coded(searchpos) != WhiteColor){
                    nextpos = searchpos;
                    currentOrder = QString("Nop");
                    LightcurrentOrder =  QString("");
                    break;
                }
            }
        }break;
    case EOrder::Black: processWall();return;
    default: case EOrder::Exception:{
        if(Coded (nextpos) >= 0 && Coded (nextpos) < 20) {
            currentOrder = QString("Nop") ;
            LightcurrentOrder = QString("");
        }else{
            QString str = QString(PietCore::CutA (-1 * (Coded (nextpos))));
            stack.push_back(PietTree(str));
            currentOrder = QString("Push ") + str;
            LightcurrentOrder = str;
        }}break;
    }
    step++;
    pos = nextpos;
    processWallCount = 0;
}

#undef STACK_TOP
#undef PROCESARITHMETICORDER

QString PietCore::printStack(){
    auto res = QString ("");
    for(auto& s : stack){
        res += s.showStack(showStackAsNumber) +QString("\n");
    }
    return res;
}

QString PietCore::printStatus(){
    QString res("");
    res += QString("w:%1 * h:%2\n").arg(w).arg(h);
    res += QString("x:%1 , y:%2\n").arg(pos.x()).arg(pos.y());
    res += QString("DP : %1\n").arg(arrowFromDP(dp));
    res += QString("CC : %1\n").arg(cc == ccR ? "R" : "L");
    res += QString("Step:%1\n").arg(step);
    res += currentOrder + QString("\n");
    //res += QString("CS:1\n");//codelsize == 1
    return res;
}

void PietCore::ExecOtherPietCore(QImage CorrectImage,QString newFilePath){
    if(CorrectImage.isNull() ){return ;}
    PietCore othercore = PietCore(outPutFunction,inPutCharFunction,inPutNumFunction);
    if(stack.size() > 0){
        vector<PietTree> PTTopstack ;
        PTTopstack.push_back( stack[stack.size() - 1]);
        othercore.setStack(PTTopstack);
        stack.pop_back();
    }
    QString currentDirPath = QDir::currentPath();
    othercore.setImage(CorrectImage,newFilePath);
    othercore.exec();
    for (auto p : othercore.getStack()){ stack.push_back(p); }
    QDir::setCurrent(currentDirPath);
}

void PietCore::ExecLib(QString LibPath){ //"L/~~.png" の "~~"部分
    if(!doesCPPPietLibExist(LibPath)) return ;
    vector<PietTree> PTTopstack ;
    if(stack.size() > 0){ PTTopstack.push_back( stack[stack.size() - 1]);stack.pop_back(); }
    ExecutingPietLibraries::functionHash[LibPath](PTTopstack);
    for (auto p : PTTopstack){ stack.push_back(p); }
}


