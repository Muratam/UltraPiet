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
#include "executingpietlibraries.h"
#include <functional>
#include <QDir>
#include <QHash>
//UltraPietの実際の命令処理をするクラス (なのでGUIには依存しません)

enum EDirectionPointer{dpR=0,dpD=1,dpL=2,dpU=3};
enum ECodelChooser{ccL=0,ccR=1};
enum class EOrder{Same=0,Push,Pop,Add,Sub,Mul,Div,Mod,Not,Great,Point,Switch,Dup,Roll,InN,InC,OutN,OutC,White,Black,Exception};

struct Point8{
    QPoint RL,RR,DL,DR,LL,LR,UL,UR;
    bool defined;
    int BlockSize;
    QPoint & getbyDPCC(EDirectionPointer dp,ECodelChooser cc){
        if(dp == dpR && cc == ccL ) return RL;
        if(dp == dpR && cc == ccR ) return RR;
        if(dp == dpD && cc == ccL ) return DL;
        if(dp == dpD && cc == ccR ) return DR;
        if(dp == dpL && cc == ccL ) return LL;
        if(dp == dpL && cc == ccR ) return LR;
        if(dp == dpU && cc == ccL ) return UL;
        if(dp == dpU && cc == ccR ) return UR;
        return RL;
    }
};
const QPoint Wall = {-1,-1};


class PietCore {
private :
    std::vector<std::vector<int>> coded;
    int Coded(const QPoint &pos){return coded[pos.x()][pos.y()];}
    std::vector<std::vector<Point8>> pos8;
    Point8 Pos8 (const QPoint &pos ){return pos8[pos.x()][pos.y()];}
    bool isCommandLineMode = false;//未実装
    int w,h;
    int step=0;
    int processWallCount=0 ;
    bool finished =false;
    std::function<void(QString)>outPutFunction;
    std::function<QChar(void)>inPutCharFunction;
    std::function<int(bool&)>inPutNumFunction;
    QString currentOrder = QString("");
    QString LightcurrentOrder = QString("");
    QPoint pos ;
    EDirectionPointer dp;
    ECodelChooser cc;
    std::vector<PietTree> stack ;
    PietTree & stackTop(){return stack[stack.size()-1];}
    bool isInImage(const QPoint & p){return p.x() >= 0 && p.y() >= 0 && p.x() < w && p.y() < h;}
    void search(QPoint me);
    void processWall();
public :
    static QString rootpath;
    static bool outputMediumFile;
    static const QRgb normalColors[3][7] ;
    static const QString normalOrders[3][7];
    static const EOrder normalEOrders[3][7];
    static QRgb getNormalColor(int nowCode,EOrder nextOrder);
    static bool isNormalColor(QRgb rgb){
        for(int i:range(3)) for(int j:range(7)) if(rgb == normalColors[i][j])return true;return false;
    }
    static QHash<QString,std::tuple<std::vector<std::vector<int>>,std::vector<std::vector<Point8>> >> Coded_Pos8_Hash ;
    static QColor getVividColor(const QColor & c){int g = DiffMax255( qGray(c.red(),c.green(),c.blue()));return QColor(g,g,g); }//int g = qGray(DiffMax255(c.red()) ,DiffMax255(c.blue()) ,DiffMax255( c.green())); return QColor(g,g,g);}
    static int DiffMax255(int c) {return c < 100 ? 255 : 0;}
    static int CutA(int c) { return ((qAlpha(c) & 0x00) << 24) | ((qRed(c) & 0xff) << 16) | ((qGreen(c) & 0xff) << 8) | (qBlue(c) & 0xff); }
    static QPoint directionFromDP(EDirectionPointer dp ){ return dp == dpR ? QPoint(1,0) :dp == dpD ? QPoint(0,1) : dp == dpL ? QPoint(-1,0) :QPoint(0,-1); }
    static QString arrowFromDP(EDirectionPointer dp ){ return QString(dp == dpR ? "→" :dp == dpD ? "↓" : dp == dpL ? "←" : "↑"); }
    static EOrder fromRelativeColor(int codedFrom,int codedTo);
public :
    bool showStackAsNumber = true;
    QString ImagePath = QString("");
    //実行前には必ずSetImageを忘れないで下さい
    PietCore (std::function<void(QString)>outPutFunction = [](QString qs){}, std::function<QChar(void)> inPutCharFunction = [](){return QChar(72);}, std::function<int(bool&)> inPutNumFunction = [](bool&b){return 0;},bool outputMedium = false);
    void init(std::function<void(QString)>outPutFunction                    ,std::function<QChar(void)>inPutCharFunction                           , std::function<int(bool&)>inPutNumFunction);
    void init(std::function<void(QString)>outPutFunction                    ,std::function<QChar(void)>inPutCharFunction                           ,std::function<int(bool&)>inPutNumFunction, const QImage & image,QString ImagePath,bool UpdateImage = false){ init(outPutFunction,inPutCharFunction,inPutNumFunction);setImage(image,ImagePath,UpdateImage);}
    void setStack(std::vector<PietTree> &nstack ){stack.clear();stack = nstack;}
    std::vector<PietTree> getStack(){return stack;}
    void ExecOtherPietCore(QImage CorrectImage, QString newFilePath);
    void ExecLib(QString LibPath);
    bool doesCPPPietLibExist (QString LibPath){if(LibPath.isEmpty())return false;else return ExecutingPietLibraries::functionHash.contains(LibPath);}
    QPoint getPos() {return pos;}
    void setPos(QPoint npos){if(npos.x() < 0 || npos.y() < 0 || npos.x() >= w || npos.y() >= h) return ;else  pos = npos;}
    void incrementDP(){dp = (EDirectionPointer)((int)dp + 1);if((int)dp >=4)dp = (EDirectionPointer)0;}
    void setDP(EDirectionPointer dp){this->dp = dp;}
    EDirectionPointer getDP() {return dp;}
    ECodelChooser getCC() {return cc;}
    bool getFinished(){return finished;}
    int getStep(){return step;}
    QString getCurrentOrder() {return currentOrder;}
    QString getLightCurrentOrder() {return LightcurrentOrder;}
    void execOneAction();
    void exec(bool MoveDirectory = true){
        if(MoveDirectory){
            QString nextDirPath = QFileInfo(ImagePath).absoluteDir().absolutePath();
            QDir::setCurrent(nextDirPath);
        }
        while (!finished){execOneAction();}
    }
    void setImage(const QImage & image, QString ImagePath,bool UpdateImage = false);
    QString printStack();
    QString printStatus();
};

#endif // PIETCORE_H
