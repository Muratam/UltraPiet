#ifndef PIETCORE_H
#define PIETCORE_H
#include <QColor>
#include <QImage>
#include <QString>
#include <vector>
#include <stack>
#include <functional>
#include "defines.h"
#include "piettree.h"
#include <functional>
//UltraPietの実際の命令処理をするクラス (なのでGUIには依存しません)

enum EDirectionPointer{dpR=0,dpD=1,dpL=2,dpU=3};
enum ECodelChooser{ccL=0,ccR=1};
enum class EOrder{Same=0,Push,Pop,Add,Sub,Mul,Div,Mod,Not,Great,Point,Switch,Dup,Roll,InN,InC,OutN,OutC,White,Black,Exception};

struct Point8{
    QPoint RL,RR,DL,DR,LL,LR,UL,UR;
    bool defined;
    int BlockSize;
    QPoint & getbyDPCC(EDirectionPointer dp,ECodelChooser cc){
#define IFDPRDLUCCLR(R_D_L_U,L_R) if(dp == dp##R_D_L_U && cc == cc##L_R ) return R_D_L_U ## L_R
        IFDPRDLUCCLR(R,L);IFDPRDLUCCLR(R,R);
        IFDPRDLUCCLR(D,L);IFDPRDLUCCLR(D,R);
        IFDPRDLUCCLR(L,L);IFDPRDLUCCLR(L,R);
        IFDPRDLUCCLR(U,L);IFDPRDLUCCLR(U,R);
        return RL;
    }
};
const QPoint Wall = {-1,-1};


class PietCore {
private :
    std::vector<std::vector<int>> coded;
    std::vector<std::vector<Point8>> pos8;
    bool isCommandLineMode = false;//未実装
    int w,h;
    int step=0;
    int processWallCount=0 ;
    bool finished =false;
    std::function<void(QString)>outPutFunction;
    QString currentOrder = QString("");
    QPoint pos ;
    EDirectionPointer dp;
    ECodelChooser cc;
    std::vector<PietTree> stack ;
    bool isInImage(const QPoint & p){return p.x() >= 0 && p.y() >= 0 && p.x() < w && p.y() < h;}
    void search(QPoint me);
    EOrder fromRelativeColor(int codedFrom,int codedTo);
    void processWall();
    QChar getInputQChar() throw (bool);
    int getInputNumber() throw (bool);
    void exec(){while (!finished){execOneAction();}} //Not For Editor
public :
    static const QRgb normalColors[3][7] ;
    static const QString normalOrders[3][7];
    static const EOrder normalEOrders[3][7];
    static bool isNormalColor(QRgb rgb){REP(i,3)REP(j,7)if(rgb == normalColors[i][j])return true;return false;}
    //static int getVividGrayScale(const QColor & c){ return (128 + qGray(c.red() , c.blue() , c.green())) % 256;}
    static int DiffMax255(int c) {return c < 128 ? 255 : 0;}
    static QColor getVividColor(const QColor & c){return QColor(DiffMax255(c.red()),DiffMax255(c.blue()),DiffMax255(c.green()));}
    static QPoint directionFromDP(EDirectionPointer dp ){ return dp == dpR ? QPoint(1,0) :dp == dpD ? QPoint(0,1) : dp == dpL ? QPoint(-1,0) :QPoint(0,-1); }
    static QString arrowFromDP(EDirectionPointer dp ){ return QString(dp == dpR ? "→" :dp == dpD ? "↓" : dp == dpL ? "←" : "↑"); }
public :
    bool showStackAsNumber = true;
    PietCore();
    void init(std::function<void(QString)>outPutFunction);
    void init(std::function<void(QString)>outPutFunction,const QImage & image){init(outPutFunction);setImage(image);}
    QPoint getPos() {return pos;}
    EDirectionPointer getDP() {return dp;}
    ECodelChooser getCC() {return cc;}
    bool getFinished(){return finished;}
    int getStep(){return step;}
    void execOneAction();
    void setImage(const QImage & image);
    QString Input = QString("this is a test"); //For Editor
    QString printStack();
    QString printStatus();
};

#endif // PIETCORE_H
