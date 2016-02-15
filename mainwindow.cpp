#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pieteditor.h"
#include "pietcore.h"
#include "defines.h"
#include <QFileDialog>
#include <QString>
#include <QDebug>

#define CB(i,j) \
    {  auto c = QColor(PietCore::normalColors[i][j]); \
       auto strrgb = QString("background-color : rgb(%1,%2,%3);\n").arg(c.red()).arg(c.green()).arg(c.blue()); \
       auto strrgbfontcolor = QString ("color : rgb(%1,%1,%1);\n").arg(PietCore::getVividGrayScale(c)); \
       ui->B ## i ## j->setStyleSheet(strrgb + strrgbfontcolor+"selection-"+strrgbfontcolor); \
       ui->B##i##j->setText( PietCore::normalOrders[i][j]); \
    }\
    connect(ui->B ## i ## j,QPushButton::clicked,[=](){ \
        auto c = PietCore::normalColors[i][j]; \
        ui->pietEditor->setPenColor(c); setEditColor(c); \
    })


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    tabifyDockWidget(ui->DockProjectFileTree,ui->DockStack);
    //tabifyDockWidget(ui->DockInput,ui->DockOutput);
    //tabifyDockWidget(ui->DockInput,ui->DockStatus);
    //splitDockWidget(ui->DockStatus,ui->DockInput,Qt::Vertical);
    //splitDockWidget(ui->DockInput,ui->DockOutput,Qt::Horizontal);
    connect(ui->actionZoom_In,SIGNAL(triggered(bool)),ui->pietEditor,SLOT(incrementZoomFactor()));
    connect(ui->actionZoom_Out,SIGNAL(triggered(bool)),ui->pietEditor,SLOT(decrementZoomFactor()));
    connect(ui->actionUndo,SIGNAL(triggered(bool)),ui->pietEditor,SLOT(undo()));
    connect(ui->action_Open,SIGNAL(triggered(bool)),this,SLOT(openImage()));
    connect(ui->action_Save,SIGNAL(triggered(bool)),this,SLOT(saveImage()));
    connect(ui->actionExec_1_Step,QAction::triggered,[=](){
        ui->pietEditor->exec1Step(ui->outputTextEdit,ui->inputTextEdit,ui->stackTextEdit,ui->StatusLabel);
    });
    connect(ui->actionExec,QAction::triggered,[=](){
        ui->pietEditor->execPiet(ui->outputTextEdit,ui->inputTextEdit,ui->stackTextEdit,ui->StatusLabel);
    });
    connect(ui->actionRestart,QAction::triggered,[=](){ui->pietEditor->execInit();});
    connect(ui->pietEditor,SIGNAL(changedPenColor(const QColor &)),this,SLOT(setEditColor(const QColor &)));
    CB(0,0);CB(0,1);CB(0,2);CB(0,3);CB(0,4);CB(0,5);CB(0,6);
    CB(1,0);CB(1,1);CB(1,2);CB(1,3);CB(1,4);CB(1,5);CB(1,6);
    CB(2,0);CB(2,1);CB(2,2);CB(2,3);CB(2,4);CB(2,5);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::openImage(){
    auto fileName = QFileDialog::getOpenFileName(this,tr("Open Image"), "", tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
    ui->pietEditor->openImage(fileName);
}

void MainWindow::saveImage(){
    auto fileName = QFileDialog::getSaveFileName(this,tr("Save Image"), "", tr("Image Files (*.png *.bmp)"));
    ui->pietEditor->saveImage(fileName);
}

void MainWindow::setEditColor(const QColor &c){
    QString strrgb = QString("background-color : rgb(%1,%2,%3);\n").arg(c.red()).arg(c.green()).arg(c.blue());
    QString strrgbfontcolor = QString ("color : rgb(%1,%1,%1);\n").arg(PietCore::getVividGrayScale(c));
    ui->BUSER->setStyleSheet(strrgb+strrgbfontcolor+"selection-"+strrgbfontcolor);//"alternate-"+strrgb+"selection-"+strrgb +
}
