#ifndef ICONEDITOR_H
#define ICONEDITOR_H
#include <QColor>
#include <QImage>
#include <QWidget>
#include <QStack>

class IconEditor : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QColor penColor READ penColor WRITE setPenColor)
    Q_PROPERTY(QImage iconImage READ iconImage WRITE setIconImage)
    Q_PROPERTY(int zoomFactor READ zoomFactor WRITE setZoomFactor) //NOTIFY?
public:
    IconEditor(QWidget * parent = 0);
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
signals :
    void changedPenColor(const QColor &color);
protected :
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
private:
    void setImagePixel(const QPoint &pos,const QRgb &rgb);
    QRgb getImagePixel(const QPoint &pos);
    QRect pixelRect(int i,int j)const;
    QColor curColor;
    QImage image;
    QStack<QImage> imageStack;
    int zoom;
};

#endif // ICONEDITOR_H
