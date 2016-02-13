#ifndef PIETCORE_H
#define PIETCORE_H
#include <QColor>
#include <QImage>
#include <QString>
#include <vector>
#include <stack>
#include "defines.h"
//UltraPietの実際の命令処理をするクラス (なのでGUIには依存しません)

// 0 3 6 9  12 15 18
// 1 4 7 10 13 16 19
// 2 5 8 11 14 17 20(将来のための)
// 他は*=-1;
enum EDirectPointer{dpR=0,dpD=1,dpL=2,dpU=3};
enum ECodelChooser{ccL=0,ccR=1};
enum class EOrder{Same=0,Push,Pop,Add,Sub,Mul,Div,Mod,Not,Great,Point,Switch,Dup,Roll,InN,InC,OutN,OutC,Exception};

struct Point8{ QPoint RL,RR,DL,DR,LL,LR,UL,UR;bool defined; };
const QPoint Wall = {-1,-1};

class PietCore {
private :
    std::vector<std::vector<int>> coded;
    std::vector<std::vector<Point8>> pos8;
    int w,h;
    QPoint pos ;
    EDirectPointer dp;
    ECodelChooser cc;
    std::vector<int> stack ;
    bool isInImage(const QPoint & p){return p.x() >= 0 && p.y() >= 0 && p.x() < w && p.y() < h;}
    void search(QPoint me);
public :
    static const QRgb normalColors[3][7] ;
    static const QString normalOrders[3][7];
    static const EOrder normalEOrders[3][7];
    static bool isNormalColor(QRgb rgb){REP(i,3)REP(j,7)if(rgb == normalColors[i][j])return true;return false;}
    static int getVividGrayScale(const QColor & c){ return (128 + qGray(c.red() , c.blue() , c.green())) % 256;}
public :
    PietCore();
    void init();
    void init(const QImage & image){init();setImage(image);}
    QPoint getPos() {return pos;}
    EDirectPointer getDP() {return dp;}
    ECodelChooser getCC() {return cc;}
    QString exec();
    void setImage(const QImage & image);
};

#endif // PIETCORE_H
