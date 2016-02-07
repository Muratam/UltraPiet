#include "iconeditor.h"
#include <QtGui>
#include <stdio.h>
#define REP(i,n) for(int i = 0 ; i < (int)(n);++i)

IconEditor::IconEditor(QWidget *parent) : QWidget (parent){
    setAttribute(Qt::WA_StaticContents);
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    curColor = Qt::red;
    zoom = 8;
    image = QImage(200,200,QImage::Format_ARGB32);
    image.fill(qRgba(255,255,255,255));
}
QSize IconEditor::sizeHint()const{
    auto size = zoom * image.size();
    if (zoom >= 3) size += QSize(1,1);
    return size;
}
void IconEditor::setPenColor(const QColor &newColor){
    curColor = newColor;
}

void IconEditor::setIconImage(const QImage &newImage){
    if(newImage != image){
        image = newImage.convertToFormat(QImage::Format_ARGB32);
        update();
        updateGeometry();
    }
}

void IconEditor::setZoomFactor(int newZoom){
    if(newZoom < 1)newZoom = 1;
    if(newZoom > 32)newZoom = 32;
    if(newZoom != zoom){
        zoom = newZoom;
        update();
        updateGeometry();
    }
}


void IconEditor::paintEvent(QPaintEvent *event){
    QPainter painter (this);
    if(zoom >= 3){
        painter.setPen(palette().foreground().color());
        REP(i,image.width()+1)  painter.drawLine(zoom*i,0,zoom*i,zoom*image.height());
        REP(i,image.height()+1) painter.drawLine(0,zoom*i,zoom*image.width(),zoom*i);
    }
    REP(i,image.width()){
        REP(j,image.height()){
            auto rect = pixelRect(i,j);
            if(!event->region().intersected(rect).isEmpty()){
                auto color = QColor::fromRgba(image.pixel(i,j));
                painter.fillRect(rect,color);
            }
        }
    }
}

QRect IconEditor::pixelRect(int i, int j) const{
    return zoom >= 3 ?
        QRect(zoom*i+1,zoom*j+1,zoom-1,zoom-1):
        QRect(zoom*i,zoom*j,zoom,zoom);
}

void IconEditor::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton)
        setImagePixel(event->pos(),true);
    else if(event->button() == Qt::RightButton)
        setImagePixel(event->pos(),false);
}
void IconEditor::mouseMoveEvent(QMouseEvent *event){
    if(event->buttons() & Qt::LeftButton)
        setImagePixel(event->pos(),true);
    else if(event->buttons() & Qt::RightButton)
        setImagePixel(event->pos(),false);
}

void IconEditor::setImagePixel(const QPoint &pos, bool opaque){
    int i = pos.x()/ zoom;
    int j = pos.y()/zoom;
    if (image.rect().contains(i,j)){
        if (opaque)image.setPixel(i,j,penColor().rgba());
        else image.setPixel(i,j,qRgba(0,0,0,0));
        update(pixelRect(i,j));
    }
}
