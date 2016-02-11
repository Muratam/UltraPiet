#ifndef PIETCORE_H
#define PIETCORE_H
#include <QWidget>

class PietCore {
    //Q_OBJECT
 public :
    static constexpr QRgb normalColors[3][7] = {
        { qRgb(255,192,192),qRgb(255,255,192),qRgb(192,255,192),qRgb(192,255,255),qRgb(192,192,255),qRgb(255,192,255),qRgb(255,255,255)},
        { qRgb(255,  0,  0),qRgb(255,255,  0),qRgb(  0,255,  0),qRgb(  0,255,255),qRgb(  0,  0,255),qRgb(255,  0,255),qRgb(  0,  0,  0)},
        { qRgb(192,  0,  0),qRgb(192,192,  0),qRgb(  0,192,  0),qRgb(  0,192,192),qRgb(  0,  0,192),qRgb(192,  0,192),qRgb(128,128,128)}
    };
};


#endif // PIETCORE_H
