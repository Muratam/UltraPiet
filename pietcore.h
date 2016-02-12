#ifndef PIETCORE_H
#define PIETCORE_H
#include <QWidget>
#include <QColor>
#include <QImage>
#include <QString>
#include <QVector>
class PietCore {
    //Q_OBJECT
 private :
    QImage image;
 public :
    static constexpr QRgb normalColors[3][7] = {
        { qRgb(255,192,192),qRgb(255,255,192),qRgb(192,255,192),qRgb(192,255,255),qRgb(192,192,255),qRgb(255,192,255),qRgb(255,255,255)},
        { qRgb(255,  0,  0),qRgb(255,255,  0),qRgb(  0,255,  0),qRgb(  0,255,255),qRgb(  0,  0,255),qRgb(255,  0,255),qRgb(  0,  0,  0)},
        { qRgb(192,  0,  0),qRgb(192,192,  0),qRgb(  0,192,  0),qRgb(  0,192,192),qRgb(  0,  0,192),qRgb(192,  0,192),qRgb(128,128,128)}
    };
    static const QString normalOrders(int i,int j){
        if(i < 0 || j < 0 || i >= 3 || j >= 7 ) return QString ("");
        QString strs[3][7] = {
            {QString("*"),   QString("add"),QString("div"),QString("great"), QString("dup"),  QString("in(c)") ,QString("")},
            {QString("push"),QString("sub"),QString("mod"),QString("point"), QString("roll"), QString("out(n)"),QString("")},
            {QString("pop"), QString("mul"),QString("not"),QString("switch"),QString("in(n)"),QString("out(c)"),QString("")}
        };
        return strs[i][j];
    }
    static int getVividGrayScale(const QColor &c) {return 255 - [](int x){return x*x;}((c.red() + c.blue() + c.green())/3);}
    void setImage(const QImage & image){ this->image = image.copy();}

};

#endif // PIETCORE_H
