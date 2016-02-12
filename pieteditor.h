#ifndef PIETEDITOR_H
#define PIETEDITOR_H
#include <QColor>
#include <QImage>
#include <QWidget>
#include <QStack>

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
    QImage image;
    QStack<QImage> imageStack;
    int zoom;
};

#endif // PIETEDITOR_H
