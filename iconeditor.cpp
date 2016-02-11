#include "iconeditor.h"
#include <QtGui>
#include <stdio.h>
#include "defines.h"

//IconEditorという名前になっていますが、Pietの描画領域Wigdetのことです...Todo::次回Pull後変更

IconEditor::IconEditor(QWidget *parent) : QWidget (parent){
    setAttribute(Qt::WA_StaticContents);
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    curColor = Qt::black;
    zoom = 16;
    image = QImage(16,16,QImage::Format_ARGB32);
    image.fill(qRgba(255,255,255,255));
    imageStack.push_back(image.copy());
}
QSize IconEditor::sizeHint()const{
    auto size = zoom * image.size();
    if (zoom >= 3) size += QSize(1,1);
    return size;
}
void IconEditor::setPenColor(const QColor &newColor){
    curColor = newColor;
    emit changedPenColor(newColor);
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

void IconEditor::undo(){
    if(imageStack.count()<= 0) return;
    image = imageStack.top();
    imageStack.pop_back();
    update();
    updateGeometry();
}

void IconEditor::mousePressEvent(QMouseEvent *event){
    imageStack.push_back(image.copy());
    if(imageStack.count() > 32) imageStack.pop_front();
    if(event->button() == Qt::LeftButton){
        setImagePixel(event->pos(),penColor().rgba());
    }else if(event->button() == Qt::RightButton){
        setPenColor( getImagePixel(event->pos()));
    }
}
void IconEditor::mouseMoveEvent(QMouseEvent *event){
    if(event->buttons() & Qt::LeftButton){
        setImagePixel(event->pos(),penColor().rgba());
    }else if(event->buttons() & Qt::RightButton){
        setPenColor( getImagePixel(event->pos()));
    }
}

void IconEditor::setImagePixel(const QPoint &pos,const QRgb &rgb){
    int i = pos.x()/ zoom;
    int j = pos.y()/zoom;
    if (image.rect().contains(i,j)){
        image.setPixel(i,j,rgb);
        update(pixelRect(i,j));
    }
}
QRgb IconEditor::getImagePixel(const QPoint &pos){
    int i = pos.x() / zoom;
    int j = pos.y() / zoom;
    if (image.rect().contains(i,j)){
        return image.pixel(i,j);
    }else return qRgb(0,0,0);
}

void IconEditor::openImage(const QString& FilePath){
    image = QImage(FilePath);
    imageStack.clear();
    imageStack.push_back(image.copy());
}
