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
    auto size = zoom * image.size() + QSize(zoom,zoom);
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
    QFont monofont =  QFontDatabase::systemFont(QFontDatabase::FixedFont);
    monofont.setPointSize(zoom / 1.8 );
    painter.setFont(monofont);
    REP(x,image.width()){
        REP(y,image.height()){
            auto rect = pixelRect(x,y);
            if(!event->region().intersected(rect).isEmpty()){
                auto color = QColor::fromRgba(image.pixel(x,y));
                painter.fillRect(rect,color);
                //painter.setPen(PietCore::getVividColor(color));
                //painter.drawText(TextRect(x,y) ,"あ");
            }
        }
    }
    painter.fillRect(zoom * image.width() , 0 ,zoom,zoom * image.height(), palette().background().color());
    painter.fillRect(0 , zoom * image.height() ,zoom * image.width(),zoom, palette().background().color());
    //painter.fillRect(zoom * core.getPos().x() , zoom * core.getPos().y() ,zoom ,zoom, QColor(255,255,255));
    painter.setPen( QPen(palette().foreground(),zoom /2 , Qt::DotLine, Qt::RoundCap));
    painter.drawPoint(zoom * core.getPos().x() + zoom /2, zoom * image.height() + zoom /2);
    painter.drawPoint(zoom * image.width()  + zoom /2, zoom * core.getPos().y()+ zoom /2);

    if(zoom > 6){
        int zm = this->zoom ;
        auto PaintText = [this,&painter,zm] (int x,int y,QString str) {
            const int Margin = 6;
            QRect TextRect ( zm * x + zm / Margin, zm * y+ zm / Margin ,zm ,zm);
            auto basecolor = QColor::fromRgba(image.pixel(x,y));
            painter.setPen(PietCore::getVividColor(basecolor));
            auto backcolor = QColor::fromRgba(image.pixel(x,y));
            painter.fillRect( this->pixelRect(x,y),backcolor);
            painter.drawText(TextRect,str);
        };
        QPoint PreAr(-1,-1);
        QPen linePen(Qt::black,1.0 + zm / 20.0 , Qt::DotLine, Qt::RoundCap);
        for(auto ar: ArrowQueue) {
            if(! ar.c.isEmpty())PaintText(ar.x(),ar.y(),ar.c);
            if(PreAr.x() != -1 && PreAr.y() != -1){//DrawLine
                if(abs(PreAr.x()- ar.x()) + abs(PreAr.y() -ar.y()) > 3 ){
                    QColor lc = PietCore::getVividColor(QColor::fromRgba(image.pixel(PreAr.x(),PreAr.y())));
                    lc.setAlpha(128); linePen.setColor(lc); painter.setPen(linePen);
                    painter.drawLine(zm * PreAr.x() + zm/2,zm * PreAr.y() + zm/2,zm * ar.x() + zm/2,zm * ar.y() + zm/2);
                }
            }
            PreAr = QPoint(ar.x(),ar.y());
        }
        PaintText(core.getPos().x(),core.getPos().y(),PietCore::arrowFromDP(core.getDP()));
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
    ArrowQueue.clear();
    if(imageStack.count()<= 0) return;
    image = imageStack.top();
    imageStack.pop_back();
    update();
    updateGeometry();
}

void PietEditor::mousePressEvent(QMouseEvent *event){
    if(isExecuting) return;
    ArrowQueue.clear();
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
    ArrowQueue.clear();
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

void PietEditor::execInit(std::function<void(QString)> outPutFunction){
    core.init(outPutFunction,image);
    ArrowQueue.clear();
    isExecuting = true;
    update();
}

void PietEditor::exec1Step (QPlainTextEdit * outputWindow,QPlainTextEdit * inputWindow,QPlainTextEdit * stackWindow,QLabel* statusLabel){
    if(!isExecuting){
        outputWindow->setPlainText(QString(""));
        execInit([outputWindow,this](QString outstr){
            outputWindow->moveCursor (QTextCursor::End);
            outputWindow->insertPlainText (outstr);
            outputWindow->moveCursor (QTextCursor::End);
        });
    }
    core.execOneAction();
    ArrowQueue.push_back(QPointAndQString( core.getPos().x(),core.getPos().y(),core.getLightCurrentOrder()));
    while(ArrowQueue.size() > ArrowQueueMaxSize)  ArrowQueue.pop_front();
    update();
    emit MovedPos (zoom * core.getPos().x(),zoom * core.getPos().y());
    stackWindow->setPlainText(core.printStack());
    statusLabel->setText(core.printStatus());
    if(core.getFinished()){
        isExecuting = false;
        MSGBOX("Debug Finished !!");
    }
}


//毎Step全状態を示すDebugと1000回に一回示すRelease
void PietEditor::execPiet(QPlainTextEdit * outputWindow, QPlainTextEdit * inputWindow, QPlainTextEdit * stackWindow, QLabel* statusLabel, bool processExentSequential){
    if(!isExecuting){
        outputWindow->setPlainText(QString(""));
        execInit([outputWindow,this,processExentSequential](QString outstr){
            outputWindow->moveCursor (QTextCursor::End);
            outputWindow->insertPlainText (outstr);
            outputWindow->moveCursor (QTextCursor::End);
        });
    }else return; //既に実行中の場合は二重起動しない
    while(! core.getFinished() && isExecuting){
        core.execOneAction();
        if(processExentSequential || core.getStep() % 1000 == 0 ){//
            stackWindow->setPlainText(core.printStack());
            statusLabel->setText(core.printStatus());
            if(processExentSequential){
                ArrowQueue.push_back(QPointAndQString( core.getPos().x(),core.getPos().y(),core.getLightCurrentOrder()));
                while(ArrowQueue.size() > ArrowQueueMaxSize)  ArrowQueue.pop_front();
                emit MovedPos (zoom * core.getPos().x(),zoom * core.getPos().y());
                update();
            }
            QApplication::processEvents();
        }
    }
    if(isExecuting){ //Escなどで妨害されず終わった場合
        stackWindow->setPlainText(core.printStack());
        statusLabel->setText(core.printStatus());
        isExecuting = false;
        if(processExentSequential) MSGBOX("Debug Finished !!");
    }
}

void PietEditor::execCancel(){
    if(!isExecuting) return;
    isExecuting = false;
    core.init([](QString s){},image);
    MSGBOX("Debug Canceled");
    update();
}
