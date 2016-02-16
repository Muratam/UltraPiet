#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pieteditor.h"
#include "pietcore.h"
#include "defines.h"
#include <QFileDialog>
#include <QScrollBar>
#include <QString>
#include <QDebug>

#define CB(i,j) \
    {  auto c = QColor(PietCore::normalColors[i][j]); \
       auto strrgb = QString("background-color : rgb(%1,%2,%3);\n").arg(c.red()).arg(c.green()).arg(c.blue()); \
       auto vivC = PietCore::getVividColor(c); \
       auto strrgbfontcolor = QString ("color : rgb(%1,%2,%3);\n").arg(vivC.red()).arg(vivC.green()).arg(vivC.blue());\
       ui->B ## i ## j->setStyleSheet(strrgb + strrgbfontcolor+"selection-"+strrgbfontcolor); \
       ui->B##i##j->setText( PietCore::normalOrders[i][j]); \
    }\
    connect(ui->B ## i ## j,QPushButton::clicked,[=](){ \
        auto c = PietCore::normalColors[i][j]; \
        ui->pietEditor->setPenColor(c); setEditColor(c); \
    })


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    this->setWindowTitle("ultrapiet");
    tabifyDockWidget(ui->DockProjectFileTree,ui->DockStack);
    //tabifyDockWidget(ui->DockInput,ui->DockOutput);
    //tabifyDockWidget(ui->DockInput,ui->DockStatus);
    //splitDockWidget(ui->DockStatus,ui->DockInput,Qt::Horizontal);
    //splitDockWidget(ui->DockInput,ui->DockOutput,Qt::Horizontal);
    connect(ui->actionZoom_In,SIGNAL(triggered(bool)),ui->pietEditor,SLOT(incrementZoomFactor()));
    connect(ui->actionZoom_Out,SIGNAL(triggered(bool)),ui->pietEditor,SLOT(decrementZoomFactor()));
    connect(ui->actionUndo,SIGNAL(triggered(bool)),ui->pietEditor,SLOT(undo()));
    connect(ui->action_Open,SIGNAL(triggered(bool)),ui->pietEditor,SLOT(openImage()));
    connect(ui->action_Save,QAction::triggered,[=](){ui->pietEditor->saveImage(false);});
    connect(ui->actionSave_as_New,QAction::triggered,[=](){ui->pietEditor->saveImage(true);});
    connect(ui->actionDebug_1_Step,QAction::triggered,[=](){
        ui->pietEditor->exec1Step(ui->outputTextEdit,ui->inputTextEdit,ui->stackTextEdit,ui->StatusLabel); });
    connect(ui->actionDebug,QAction::triggered,[=](){
        ui->pietEditor->execPiet(ui->outputTextEdit,ui->inputTextEdit,ui->stackTextEdit,ui->StatusLabel,true);});
    connect(ui->actionRelease,QAction::triggered,[=](){
        ui->pietEditor->execPiet(ui->outputTextEdit,ui->inputTextEdit,ui->stackTextEdit,ui->StatusLabel,false);});
    connect(ui->changeAsNumber,     QPushButton::clicked,[=](){ui->pietEditor->ChangeShowStackAsNumber(ui->stackTextEdit);});
    connect(ui->actionShow_As_Number,QAction::triggered,[=](){ui->pietEditor->ChangeShowStackAsNumber(ui->stackTextEdit);});
    connect(ui->actionCancel,QAction::triggered,[=](){ui->pietEditor->execCancel();});
    connect(ui->pietEditor,SIGNAL(changedPenColor(const QColor &)),this,SLOT(setEditColor(const QColor &)));
    connect(ui->pietEditor,PietEditor::MovedPos,[=](int x,int y){
        ui->scrollArea->horizontalScrollBar()->setValue(x - ui->scrollArea->viewport()->width()  / 2);
        ui->scrollArea->verticalScrollBar()->setValue(y - ui->scrollArea->viewport()->height() / 2);
    });
    CB(0,0);CB(0,1);CB(0,2);CB(0,3);CB(0,4);CB(0,5);CB(0,6);
    CB(1,0);CB(1,1);CB(1,2);CB(1,3);CB(1,4);CB(1,5);CB(1,6);
    CB(2,0);CB(2,1);CB(2,2);CB(2,3);CB(2,4);CB(2,5);
}

MainWindow::~MainWindow(){
    delete ui;
}
void MainWindow::setEditColor(const QColor &c){
    QString strrgb = QString("background-color : rgb(%1,%2,%3);\n").arg(c.red()).arg(c.green()).arg(c.blue());
    auto vivC = PietCore::getVividColor(c);
    QString strrgbfontcolor = QString ("color : rgb(%1,%2,%3);\n").arg(vivC.red()).arg(vivC.green()).arg(vivC.blue());
    ui->BUSER->setStyleSheet(strrgb+strrgbfontcolor+"selection-"+strrgbfontcolor);//"alternate-"+strrgb+"selection-"+strrgb +
}
