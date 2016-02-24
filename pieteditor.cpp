#include "pieteditor.h"
#include <QtWidgets>
#include <stdio.h>
#include "defines.h"

PietEditor::PietEditor(QWidget *parent) : QWidget (parent) {
    setAttribute(Qt::WA_StaticContents);
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    curColor = QColor(PietCore::normalColors[0][0]);
    zoom = 16;
    image = QImage(16,16,QImage::Format_RGB32);
    image.fill(qRgba(255,255,255,255));
    imageStack.push_back(ImageOperateLog( image.copy(),QPoint(0,0),dpR));
    setAcceptDrops(true);
    core.setImage(image,QString(""));
    loadedFilePath = QString("");
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
        image = newImage.convertToFormat(QImage::Format_RGB32);
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
    int zm = this->zoom ;
    auto PaintText = [this,&painter,zm] (int x,int y,QString str,bool fill = true) {
        const int Margin = 6;
        QRect TextRect ( zm * x + zm / Margin, zm * y+ zm / Margin ,zm ,zm);
        auto basecolor = QColor::fromRgba(image.pixel(x,y));
        painter.setPen(PietCore::getVividColor(basecolor));
        auto backcolor = QColor::fromRgba(image.pixel(x,y));
        if(fill) painter.fillRect( this->pixelRect(x,y),backcolor);
        painter.drawText(TextRect,str);
    };
    if(zoom >= 3){ //grid line
        painter.setPen(palette().foreground().color());
        for(auto i:range(image.width()+1 ))  painter.drawLine(zoom*i,0,zoom*i,zoom*image.height());
        for(auto i:range(image.height()+1))  painter.drawLine(0,zoom*i,zoom*image.width(),zoom*i);
    }
    QFont monofont =  QFontDatabase::systemFont(QFontDatabase::FixedFont);
    monofont.setPointSize(zoom / 1.8 );
    painter.setFont(monofont);
    for(auto x : range(image.width())){
        for(auto y:range(image.height())){
            auto rect = pixelRect(x,y);
            if(!event->region().intersected(rect).isEmpty()){
                auto color = QColor::fromRgba(image.pixel(x,y));
                if(zoom > 6 && !PietCore::isNormalColor(image.pixel(x,y))){
                    painter.setPen(PietCore::getVividColor(color));
                    int rgb = image.pixel(x,y); //ARGB,強制的に A => 0 として処理
                    if(32 == PietCore::CutA(rgb))PaintText(x,y ,QString("SP"));
                    else PaintText(x,y ,QString(rgb));
                }else{
                    painter.fillRect(rect,color);
                }
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
        QPoint PreAr(-1,-1);
        QPen linePen(Qt::black,1.0 + zm / 20.0 , Qt::DotLine, Qt::RoundCap);
        for(auto& ar: ArrowQueue) {
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
        if(!this->isExecuting){
            for(auto& state : imageStack){
                if(!state.order.isEmpty()){
                    PaintText(state.pos.x(),state.pos.y(),state.order);
                }
            }
        }
        PaintText(core.getPos().x(),core.getPos().y(),PietCore::arrowFromDP(core.getDP()),false);

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

void PietEditor::keyPressEvent( QKeyEvent *event ){
    if(isExecuting )return;
    QRgb nowRGB = image.pixel(core.getPos().x(),core.getPos().y());
    int nowcode = -1;
    for(auto i:range(3)) for(auto j:range(6)) if(nowRGB == PietCore::normalColors[i][j]){nowcode = 3*j+i; goto MATCHED;}
    ;MATCHED:;
    if(nowcode == -1 ){nowcode = 0;}
    auto CommandCore = [nowcode,this](EOrder nextOrder,QString strorder){
        QPoint prepos = core.getPos();
        core.setPos(prepos + PietCore::directionFromDP(core.getDP()));
        if(prepos != core.getPos()){
            imageStack.push_back(ImageOperateLog( image.copy(),core.getPos(),core.getDP(),strorder));
            if(imageStack.size() > StackMaxSize()) imageStack.pop_front();
            setImagePixel(zoom * core.getPos(),PietCore::getNormalColor(nowcode,nextOrder));
            emit MovedPos (zoom * core.getPos().x(),zoom * core.getPos().y());
        }
    };
    switch( event->key() ){
    case Qt::Key_P:{
        if (event->modifiers()!=Qt::ShiftModifier)
            CommandCore(EOrder::Push,QString("Ｐ"));
        else CommandCore(EOrder::Pop,QString("♪"));
        }break;
    case Qt::Key_Plus:case Qt::Key_Semicolon:
        CommandCore(EOrder::Add,QString("＋")); break;
    case Qt::Key_Minus:
        CommandCore(EOrder::Sub,QString("-")); break;
    case Qt::Key_Colon: case Qt::Key_Asterisk:
        CommandCore(EOrder::Mul,QString("×")); break;
    case Qt::Key_Slash:
        CommandCore(EOrder::Div,QString("÷")); break;
    case Qt::Key_Percent:case Qt::Key_5:
        CommandCore(EOrder::Mod,QString("％")); break;
    case Qt::Key_Exclam: case Qt::Key_1:
        CommandCore(EOrder::Not,QString("！")); break;
    case Qt::Key_Greater: case Qt::Key_Period:
        CommandCore(EOrder::Great,QString("＜")); break;
    case Qt::Key_At:
        CommandCore(EOrder::Point,QString("＠")); break;
    case Qt::Key_AsciiCircum :
        CommandCore(EOrder::Switch,QString("＾")); break;
    case Qt::Key_D :
        CommandCore(EOrder::Dup,QString("Ｄ")); break;
    case Qt::Key_R :
        CommandCore(EOrder::Roll,QString("Ｒ")); break;
    case Qt::Key_I :{
        if (event->modifiers()==Qt::ShiftModifier)
             CommandCore(EOrder::InN,QString("IN"));
        else CommandCore(EOrder::InC,QString("ic"));
        }break;
    case Qt::Key_O :{
        if (event->modifiers()==Qt::ShiftModifier)
            CommandCore(EOrder::OutN,QString("ON"));
        else CommandCore(EOrder::OutC,QString("oc"));
        }break;
    case Qt::Key_Space :
        CommandCore(EOrder::White,QString("　")); break;
    case Qt::Key_B :
        CommandCore(EOrder::Black,QString("　")); break;
    case Qt::Key_Return:{
        if (event->modifiers()==Qt::ShiftModifier){
            core.incrementDP();core.incrementDP();core.incrementDP();
        }else {core.incrementDP();
        }}break; //Change DP
    case Qt::Key_Down : case Qt::Key_Up :case Qt::Key_Right: case Qt::Key_Left:{
        QPoint Direction(0,0) ;
        switch( event->key() ){
            case Qt::Key_Down : Direction = QPoint(0,1) ;break;
            case Qt::Key_Up   : Direction = QPoint(0,-1);break;
            case Qt::Key_Right: Direction = QPoint(1,0) ;break;
            case Qt::Key_Left : Direction = QPoint(-1,0);break;
        }
        QPoint prepos = core.getPos();
        core.setPos (core.getPos() + Direction);
        if (event->modifiers()==Qt::ShiftModifier){
            imageStack.push_back(ImageOperateLog( image.copy(),core.getPos(),core.getDP()));
            if(imageStack.size() > StackMaxSize()) imageStack.pop_front();
            setImagePixel(zoom *core.getPos(),image.pixel(prepos.x(),prepos.y()));
        } emit MovedPos (zoom * core.getPos().x(),zoom * core.getPos().y());

        }break; //Arrow
    case Qt::Key_Delete:{
        imageStack.push_back(ImageOperateLog( image.copy(),core.getPos(),core.getDP(),QString("　")));
            setImagePixel(zoom * core.getPos(),PietCore::getNormalColor(0,EOrder::White));
        }break;
    case Qt::Key_U: { //insert unicode
            bool ok = false;
            QString unicodes = QInputDialog::getText(this, tr("QInputDialog::getText()"), tr("Insert Unicode"), QLineEdit::Normal, QString("あ"), &ok);
            if (!ok || unicodes.isEmpty()) break ;
            QChar pre(0);
            for(QChar c : unicodes){
                QPoint prepos = core.getPos();
                core.setPos(prepos + PietCore::directionFromDP(core.getDP()));
                if(prepos != core.getPos()){
                    if(c == pre){
                        imageStack.push_back(ImageOperateLog( image.copy(),core.getPos(),core.getDP(),QString("")));
                        setImagePixel(zoom * core.getPos(), PietCore::normalColors[0][0] );
                        if(imageStack.size() > StackMaxSize()) imageStack.pop_front();
                        emit MovedPos (zoom * core.getPos().x(),zoom * core.getPos().y());
                        prepos = core.getPos();
                        core.setPos(prepos + PietCore::directionFromDP(core.getDP()));
                    }
                    imageStack.push_back(ImageOperateLog( image.copy(),core.getPos(),core.getDP(),QString("")));
                    int ic = c.unicode();
                    setImagePixel(zoom * core.getPos(), ic );
                    if(imageStack.size() > StackMaxSize()) imageStack.pop_front();
                    emit MovedPos (zoom * core.getPos().x(),zoom * core.getPos().y());
                }
                pre = c;
            }
        }  break;
    case Qt::Key_Backspace: undo();break;
    }
    /*
    普通の移動 : ←↓↑→
    push,pop,+,-,*,/,%,!,>,point,switch,dup,roll,inN,inC,oN,oC,White,Black,NonNormal,Same,DP
    p,P,+,-,*,/,%,!,>,@,^,d,r,I,i,O,o,Space,b,ほげ,Shift+←↓↑→,Return,
    数学的演算記号はShiftはどっちでもいい
    */
    update();
}


QRect PietEditor::pixelRect(int i, int j) const{
    return zoom >= 3 ?
        QRect(zoom*i+1,zoom*j+1,zoom-1,zoom-1):
        QRect(zoom*i,zoom*j,zoom,zoom);
}

void PietEditor::undo(){
    if(isExecuting) return;
    ArrowQueue.clear();
    if(imageStack.size()<= 0) return;
    ImageOperateLog Ope = imageStack.back();
    image = Ope.image;
    core.setPos(Ope.pos);
    core.setDP(Ope.dp);
    imageStack.pop_back();
    if(imageStack.size() > 0){
        ImageOperateLog Ope2 = imageStack.back();
        core.setPos(Ope2.pos);
        core.setDP(Ope2.dp);
    }
    update();
    updateGeometry();
}

void PietEditor::mousePressEvent(QMouseEvent *event){
    if(isExecuting) return;
    ArrowQueue.clear();
    setFocus();
    if(zoom >= 1)core.setPos(QPoint(event->pos().x() / zoom,event->pos().y()/zoom));
    imageStack.push_back(ImageOperateLog( image.copy(),core.getPos(),core.getDP()));
    if(imageStack.size() > StackMaxSize()) imageStack.pop_front();
    if(event->button() == Qt::LeftButton){
        setImagePixel(event->pos(),penColor().rgba());
    }else if(event->button() == Qt::RightButton){
        setPenColor( getImagePixel(event->pos()));
    }
    update();
}
void PietEditor::mouseMoveEvent(QMouseEvent *event){
    if(isExecuting) return;
    //if(hadnotFocused) return;
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
    if(imageStack.size() > 1) {
        QMessageBox msgBox(this);
        msgBox.setText(tr("Open Image ? \n (Existing Image Will Be Discarded.)"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::No);
        int res = msgBox.exec();
        if (res == QMessageBox::No) return;
    }
    ArrowQueue.clear();
    if(FilePath.isEmpty() || FilePath.isNull())
        FilePath = QFileDialog::getOpenFileName(this,tr("Open Image"), "", tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
    if(FilePath.isEmpty() || FilePath.isNull())return;
    auto loadedimage = QImage(FilePath);
    if(loadedimage.isNull() ){MSGBOX("Invalid Image");return;}
    loadedFilePath = FilePath;
    image = loadedimage;
    imageStack.clear();
    imageStack.push_back(ImageOperateLog( image.copy(),core.getPos(),core.getDP()));
    if(imageStack.size() > StackMaxSize()) imageStack.pop_front();
    core.setPos(QPoint(0,0));
    core.setDP(dpR);
    core.setImage(image,loadedFilePath);
    update();
    updateGeometry();
    emit OpenedImage(loadedFilePath);
}

void PietEditor::resize(){
    if(isExecuting) return;
    if(imageStack.size() > 1) {
        QMessageBox msgBox(this);
        msgBox.setText(tr("Resize Image ? \n (Existing Edit History May Be Discarded.)"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::No);
        int res = msgBox.exec();
        if (res == QMessageBox::No) return;
    }
    int gotw = 16, goth = 16;
    bool ok = false;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getInt()"), tr("Input Width:"), QLineEdit::Normal, "16", &ok);
    if (ok && !text.isEmpty())  gotw = text.toInt();
    else return;
    text = QInputDialog::getText(this, tr("QInputDialog::getInt()"), tr("Input Height:"), QLineEdit::Normal, "16", &ok);
    if (ok && !text.isEmpty())  goth = text.toInt();
    else return;
    if(gotw <= 0 || goth <= 0) {MSGBOX("INVALID SIZE"); return;}
    QImage newImage(gotw,goth,QImage::Format_RGB32);
    newImage.fill(qRgba(255,255,255,255));
    int minw = std::min(gotw,image.width());
    int minh = std::min(goth,image.height());
    for(auto x:range( minw)) for(auto y:range(minh)){newImage.setPixel(x,y,image.pixel(x,y));}
    image = newImage;
    //if(image.size().width() > gotw || image.size().height > goth){
    core.setPos(QPoint(0,0));
    core.setDP(dpR);
    imageStack.clear();
    imageStack.push_back(ImageOperateLog( image.copy(),QPoint(0,0),dpR));
    core.setImage(image,loadedFilePath);
    ArrowQueue.clear();
    update();
    updateGeometry();
}

void PietEditor::newImage(){
    if(isExecuting) return;
    if(imageStack.size() > 1) {
        QMessageBox msgBox(this);
        msgBox.setText(tr("Create New Image ? \n (Existing Image Will Be Discarded.)"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::No);
        int res = msgBox.exec();
        if (res == QMessageBox::No) return;
    }
    int gotw = 16, goth = 16;
    bool ok = false;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getInt()"), tr("Input Width:"), QLineEdit::Normal, "16", &ok);
    if (ok && !text.isEmpty())  gotw = text.toInt();
    else return;
    text = QInputDialog::getText(this, tr("QInputDialog::getInt()"), tr("Input Height:"), QLineEdit::Normal, "16", &ok);
    if (ok && !text.isEmpty())  goth = text.toInt();
    else return;
    if(gotw <= 0 || goth <= 0) {MSGBOX("INVALID SIZE"); return;}
    zoom = 16;
    image = QImage(gotw,goth,QImage::Format_RGB32);
    image.fill(qRgba(255,255,255,255));
    imageStack.clear();
    imageStack.push_back(ImageOperateLog( image.copy(),QPoint(0,0),dpR));
    loadedFilePath = QString("");
    core.setPos(QPoint(0,0));
    core.setDP(dpR);
    core.setImage(image,QString(""));
    ArrowQueue.clear();
    update();
    updateGeometry();
    //emit OpenedImage(loadedFilePath);
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
    emit OpenedImage(loadedFilePath);
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}


void PietEditor::SetUpInitialStack(QPlainTextEdit * stackWindow){
    if(isExecuting) return;
    bool ok = false;
    QString defaultstr = stackWindow->toPlainText().isEmpty() ? QString("[hoge[huga]]"):stackWindow->toPlainText();
    QString unicodes = QInputDialog::getText(this, tr("QInputDialog::getText()"), tr("SetUp Initial Stack"), QLineEdit::Normal,defaultstr , &ok);
    if (!ok || unicodes.isEmpty()) return;
    initialStack.clear();
    initialStack.push_back( PietTree::fromString(unicodes));
    core.setStack(initialStack);
    stackWindow->setPlainText(core.printStack());
}

void PietEditor::execInit(QPlainTextEdit * outputWindow,QPlainTextEdit * inputWindow,QPlainTextEdit * stackWindow){
    core.init(
        [outputWindow,this](QString outstr){
            outputWindow->moveCursor (QTextCursor::End);
            outputWindow->insertPlainText (outstr);
            outputWindow->moveCursor (QTextCursor::End);
        },[inputWindow,this](){
            if(inputWindow->toPlainText().isEmpty())MSGBOX("NO INPUT CHARS");
            while(inputWindow->toPlainText().isEmpty()){
                this->isWaitingInput = true;
                QApplication::processEvents();
                if(isWaintingInputThenExecCancelSignal){
                    isWaitingInput = false;
                    return QChar(0);
                }
            }
            QString Input = inputWindow->toPlainText();
            QChar res = Input.at(0);
            Input.remove(0,1);
            inputWindow->setPlainText(Input);
            isWaitingInput = false;
            return res;
        },[inputWindow,this](bool &Miss){
            if(inputWindow->toPlainText().isEmpty())MSGBOX("NO INPUT CHARS");
            while(inputWindow->toPlainText().isEmpty()){
                this->isWaitingInput = true;
                QApplication::processEvents();
                if(isWaintingInputThenExecCancelSignal){
                    isWaitingInput = false;
                    Miss = true;
                    return 0;
                }
            }//空ではなければ処理
            QString Input = inputWindow->toPlainText();
            QRegExp rx("^\\s*-?\\d+");
            if(-1 == rx.indexIn(Input)) { Miss = true; return 0;}
            QString Match = rx.cap();
            int m = Match.toInt();
            Input.remove(0,Match.length());
            inputWindow->setPlainText(Input);
            isWaitingInput = false;
            return m;
        },image,loadedFilePath);
    ArrowQueue.clear();
    isExecuting = true;
    InitialInput = inputWindow->toPlainText();    
    stackWindow->setPlainText(core.printStack());
    isWaintingInputThenExecCancelSignal = isWaitingInput = false;
    core.setStack(initialStack);
    stackWindow->setPlainText(core.printStack());
    update();
}

void PietEditor::exec1Step (QPlainTextEdit * outputWindow,QPlainTextEdit * inputWindow,QPlainTextEdit * stackWindow,QLabel* statusLabel){
    if(!isExecuting){
        outputWindow->setPlainText(QString(""));
        execInit(outputWindow,inputWindow,stackWindow);
    }else if(isWaitingInput)return;
    core.execOneAction();
    ArrowQueue.push_back(QPointAndQString( core.getPos().x(),core.getPos().y(),core.getLightCurrentOrder()));
    while(ArrowQueue.size() > ArrowQueueMaxSize)  ArrowQueue.pop_front();
    update();
    emit MovedPos (zoom * core.getPos().x(),zoom * core.getPos().y());
    stackWindow->setPlainText(core.getCurrentOrder() + QString("\n") + core.printStack());
    statusLabel->setText(core.printStatus());
    if(core.getFinished()){
        isExecuting = false;
        MSGBOX("Debug Finished !!");
        inputWindow->setPlainText(InitialInput);
        core.setStack(initialStack);
        stackWindow->setPlainText(core.printStack());
    }
    if(isWaintingInputThenExecCancelSignal) execCancel(inputWindow,stackWindow);
}


//毎Step全状態を示すDebugと1000回に一回示すRelease
void PietEditor::execPiet(QPlainTextEdit * outputWindow, QPlainTextEdit * inputWindow, QPlainTextEdit * stackWindow, QLabel* statusLabel, bool processExentSequential){
    if(!isExecuting){
        outputWindow->setPlainText(QString(""));
        execInit(outputWindow,inputWindow,stackWindow);
    }else return; //既に実行中の場合は二重起動しない
    while(! core.getFinished() && isExecuting){
        core.execOneAction();
        if(isWaintingInputThenExecCancelSignal) break;
        if(processExentSequential || core.getStep() % 1000 == 0 ){//
            stackWindow->setPlainText(core.getCurrentOrder() + QString("\n") + core.printStack());
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
        inputWindow->setPlainText(InitialInput);
        core.setStack(initialStack);
        stackWindow->setPlainText(core.printStack());
    }
}

void PietEditor::execCancel( QPlainTextEdit * inputWindow,QPlainTextEdit * stackWindow){
    if(!isExecuting) return;
    if(isWaitingInput){ isWaintingInputThenExecCancelSignal = true; return;}
    isExecuting = false;
    core.init([](QString s){},[](){return QChar(72);},[](bool & b){return 0;},image,loadedFilePath);
    MSGBOX("Debug Canceled");
    inputWindow->setPlainText(InitialInput);
    core.setStack(initialStack);
    stackWindow->setPlainText(core.printStack());
    update();
}
