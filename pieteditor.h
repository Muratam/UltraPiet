#ifndef PIETEDITOR_H
#define PIETEDITOR_H
#include <QColor>
#include <QImage>
#include <QWidget>
#include <QStack>
#include <deque>
#include <functional>
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
    bool showStackAsNumber() const{return core.showStackAsNumber;}
public slots :
    void setZoomFactor(int newZoom);
    void incrementZoomFactor(){setZoomFactor(zoom+1);}
    void decrementZoomFactor(){setZoomFactor(zoom-1);}
    void setIconImage (const QImage &newImage);
    void setPenColor(const QColor &newColor);
    void undo();
    void openImage(QString FilePath = QString (""));
    void saveImage(bool asNew);
    void execPiet(QPlainTextEdit * outputWindow,QPlainTextEdit * inputWindow,QPlainTextEdit * stackWindow,QLabel* statusLabel,bool processExentSequential);
    void exec1Step(QPlainTextEdit * outputWindow,QPlainTextEdit * inputWindow,QPlainTextEdit * stackWindow,QLabel* statusLabel);
    void execCancel();
    void ChangeShowStackAsNumber(QPlainTextEdit * stackWindow){core.showStackAsNumber = !core.showStackAsNumber ;stackWindow->setPlainText(core.printStack());}
signals :
    void changedPenColor(const QColor &color);
    void MovedPos(int x,int y);
protected :
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void setImagePixel(const QPoint &pos,const QRgb &rgb);
    QRgb getImagePixel(const QPoint &pos);
    void execInit(std::function<void(QString)>outPutFunction);
    QRect pixelRect(int i,int j)const;
    QColor curColor;
    PietCore core;
    QImage image;
    class QPointAndQString :public QPoint{public :QString c; QPointAndQString(int x,int y,QString cc):QPoint(x,y){c =cc;}};
    std::deque<QPointAndQString> ArrowQueue;
    const int ArrowQueueMaxSize = 256;
    QString loadedFilePath = QString("");
    QStack<QImage> imageStack; // For Undo
    int zoom;
    bool isExecuting = false;
};

#endif // PIETEDITOR_H
