#include "pieteditor.h"
#include <QtWidgets>
#include <stdio.h>
#include "defines.h"

PietEditor::PietEditor(QWidget *parent) : QWidget (parent){
    setAttribute(Qt::WA_StaticContents);
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    curColor = Qt::black;
    zoom = 16;
    image = QImage(16,16,QImage::Format_ARGB32);
    image.fill(qRgba(255,255,255,255));
    imageStack.push_back(image.copy());
    setAcceptDrops(true);
    core.setImage(image);
}
QSize PietEditor::sizeHint()const{
    auto size = zoom * image.size();
    if (zoom >= 3) size += QSize(1,1);
    return size;
}
void PietEditor::setPenColor(const QColor &newColor){
    curColor = newColor;
    emit changedPenColor(newColor);
}

void PietEditor::setIconImage(const QImage &newImage){
    if(isExecuting) return;
    if(newImage != image){
        image = newImage.convertToFormat(QImage::Format_ARGB32);
        update();
        updateGeometry();
    }
}

void PietEditor::setZoomFactor(int newZoom){
    if(newZoom < 1)newZoom = 1;
    if(newZoom > 32)newZoom = 32;
    if(newZoom != zoom){
        zoom = newZoom;
        update();
        updateGeometry();
    }
}


void PietEditor::paintEvent(QPaintEvent *event){
    QPainter painter (this);
    if(zoom >= 3){ //grid line
        painter.setPen(palette().foreground().color());
        REP(i,image.width()+1)  painter.drawLine(zoom*i,0,zoom*i,zoom*image.height());
        REP(i,image.height()+1) painter.drawLine(0,zoom*i,zoom*image.width(),zoom*i);    
    }
    painter.setFont(QFont("Arial",zoom / 1.5 ));

    REP(i,image.width()){
        REP(j,image.height()){
            auto rect = pixelRect(i,j);
            if(!event->region().intersected(rect).isEmpty()){
                auto color = QColor::fromRgba(image.pixel(i,j));
                painter.fillRect(rect,color);
            }
        }
    }
    if(zoom > 6){
        painter.setPen(QColor(0,0,0));
        auto arrow = PietCore::arrowFromDP(core.getDP());
        painter.drawText(pixelRect(core.getPos().x(),core.getPos().y()),arrow);
    }
}
void PietEditor::dragEnterEvent(QDragEnterEvent *event){
    if(isExecuting) return;
    if(event->mimeData()->hasFormat("text/uri-list")) event->acceptProposedAction();
}

void PietEditor::dropEvent(QDropEvent *event){
    if(isExecuting) return;
    if(!event->mimeData()->hasFormat("text/uri-list"))return;
    openImage(event->mimeData()->urls().first().toLocalFile() );
}

QRect PietEditor::pixelRect(int i, int j) const{
    return zoom >= 3 ?
        QRect(zoom*i+1,zoom*j+1,zoom-1,zoom-1):
        QRect(zoom*i,zoom*j,zoom,zoom);
}

void PietEditor::undo(){
    if(isExecuting) return;
    if(imageStack.count()<= 0) return;
    image = imageStack.top();
    imageStack.pop_back();
    update();
    updateGeometry();
}

void PietEditor::mousePressEvent(QMouseEvent *event){
    if(isExecuting) return;
    imageStack.push_back(image.copy());
    if(imageStack.count() > 32) imageStack.pop_front();
    if(event->button() == Qt::LeftButton){
        setImagePixel(event->pos(),penColor().rgba());
    }else if(event->button() == Qt::RightButton){
        setPenColor( getImagePixel(event->pos()));
    }
}
void PietEditor::mouseMoveEvent(QMouseEvent *event){
    if(isExecuting) return;
    if(event->buttons() & Qt::LeftButton){
        setImagePixel(event->pos(),penColor().rgba());
    }else if(event->buttons() & Qt::RightButton){
        setPenColor( getImagePixel(event->pos()));
    }
}

void PietEditor::setImagePixel(const QPoint &pos,const QRgb &rgb){
    int i = pos.x()/ zoom;
    int j = pos.y()/zoom;
    if (image.rect().contains(i,j)){
        image.setPixel(i,j,rgb);
        update(pixelRect(i,j));
    }
}
QRgb PietEditor::getImagePixel(const QPoint &pos){
    int i = pos.x() / zoom;
    int j = pos.y() / zoom;
    if (image.rect().contains(i,j)){
        return image.pixel(i,j);
    }else return qRgb(0,0,0);
}

void PietEditor::openImage(QString FilePath ){
    if(isExecuting) return;
    if(FilePath.isEmpty() || FilePath.isNull())
        FilePath = QFileDialog::getOpenFileName(this,tr("Open Image"), "", tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
    if(FilePath.isEmpty() || FilePath.isNull())return;
    auto loadedimage = QImage(FilePath);
    if(loadedimage.isNull() ){MSGBOX("Invalid Image");return;}
    loadedFilePath = FilePath;
    image = loadedimage;
    imageStack.clear();
    imageStack.push_back(image.copy());
    update();
    updateGeometry();
}

void PietEditor::saveImage(bool asNew){
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString filePath =
        (!asNew && !loadedFilePath.isEmpty() && !loadedFilePath.isNull()) ? loadedFilePath :
        QFileDialog::getSaveFileName(this,tr("Save Image"), "", tr("Image Files (*.png *.bmp)"));
    if(filePath.isEmpty() || filePath.isNull()){ QApplication::setOverrideCursor(Qt::ArrowCursor); return;}
    if(!image.save(filePath)){QApplication::setOverrideCursor(Qt::ArrowCursor);MSGBOX("Cannot Save...");return;}
    loadedFilePath = filePath;
    Sleep(100);
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

//画面無視で速度的なあれ
void PietEditor::execPiet(QPlainTextEdit * outputWindow,QPlainTextEdit * inputWindow,QPlainTextEdit * stackWindow,QLabel* statusLabel){
    if(!isExecuting) execInit();
    //core.Input = inputWindow->toPlainText();
    while(! core.getFinished() && isExecuting){
        core.execOneAction();
        if(core.getStep() % 10000 == 0 ){ //一万回に一回GUIに戻って処理する
            outputWindow->setPlainText(core.Output);
            stackWindow->setPlainText(core.printStack());
            statusLabel->setText(core.printStatus());
            QApplication::processEvents();
        }
    }
    if(isExecuting){ //Escなどで妨害されず終わった場合
        outputWindow->setPlainText(core.Output);
        stackWindow->setPlainText(core.printStack());
        statusLabel->setText(core.printStatus());
        isExecuting = false;
    }
}
void PietEditor::execInit(){
    core.init(image);
    isExecuting = true;
    update();
}
void PietEditor::execCancel(){
    if(!isExecuting) return;
    isExecuting = false;
    core.init(image);
    MSGBOX("Debug Canceled");
    update();
}


void PietEditor::exec1Step (QPlainTextEdit * outputWindow,QPlainTextEdit * inputWindow,QPlainTextEdit * stackWindow,QLabel* statusLabel){
    if(!isExecuting) execInit();
    //core.Input = inputWindow->toPlainText();
    QPoint prepos = core.getPos();
    core.execOneAction();
    update(pixelRect(prepos.x(),prepos.y()));
    update(pixelRect(core.getPos().x(),core.getPos().y()));
    outputWindow->setPlainText(core.Output);
    stackWindow->setPlainText(core.printStack());
    statusLabel->setText(core.printStatus());
    if(core.getFinished())isExecuting = false;
}
