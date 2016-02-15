#ifndef PIETEDITOR_H
#define PIETEDITOR_H
#include <QColor>
#include <QImage>
#include <QWidget>
#include <QStack>
#include <QPlainTextEdit>
#include "pietcore.h"

class PietEditor : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QColor penColor READ penColor WRITE setPenColor)
    Q_PROPERTY(QImage iconImage READ iconImage WRITE setIconImage)
    Q_PROPERTY(int zoomFactor READ zoomFactor WRITE setZoomFactor) //NOTIFY?
public:
    PietEditor(QWidget * parent = 0);
    int zoomFactor()const {return zoom;}
    QImage iconImage()const{return image;}
    QColor penColor() const {return curColor;}
    QSize sizeHint() const;
public slots :
    void setZoomFactor(int newZoom);
    void incrementZoomFactor(){setZoomFactor(zoom+1);}
    void decrementZoomFactor(){setZoomFactor(zoom-1);}
    void setIconImage (const QImage &newImage);
    void setPenColor(const QColor &newColor);
    void undo();
    void openImage(const QString& filePath);
    void saveImage(const QString& filePath);
    void execPiet(QPlainTextEdit * outputWindow,QPlainTextEdit * inputWindow,QPlainTextEdit * stackWindow,QLabel* statusLabel);
    void exec1Step(QPlainTextEdit * outputWindow,QPlainTextEdit * inputWindow,QPlainTextEdit * stackWindow,QLabel* statusLabel);
    void execInit();
signals :
    void changedPenColor(const QColor &color);
protected :
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
private:
    void setImagePixel(const QPoint &pos,const QRgb &rgb);
    QRgb getImagePixel(const QPoint &pos);
    QRect pixelRect(int i,int j)const;
    QColor curColor;
    PietCore core;
    QImage image;
    QStack<QImage> imageStack; // For Undo
    int zoom;
    bool isExecuting = false;
};

#endif // PIETEDITOR_H
