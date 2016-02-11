#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iconeditor.h"
#include "pietcore.h"
#include "defines.h"
#include <QFileDialog>
#include <QString>
#include <QDebug>

#define CB(i,j) connect(ui->B ## i ## j,QPushButton::clicked,[=](){auto c = PietCore::normalColors[i][j];ui->PietEditor->setPenColor(c); setEditColor(c);})
    /*    ui->PietEditor->setPenColor(PietCore::NormalColors[i][j]); \
        QColor c(PietCore::NormalColors[i][j]); \
        QString strrgb = QString("background-color : rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue()); \
        ui->BUSER->setStyleSheet(strrgb+" \nalternate-"+strrgb+"\nselection-"+strrgb); \
    })*/


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    tabifyDockWidget(ui->DockProjectFileTree,ui->DockStack);
    //tabifyDockWidget(ui->DockInput,ui->DockOutput);
    //tabifyDockWidget(ui->DockInput,ui->DockStatus);
    //splitDockWidget(ui->DockStatus,ui->DockInput,Qt::Vertical);
    //splitDockWidget(ui->DockInput,ui->DockOutput,Qt::Horizontal);
    //connect(ui->sizespinBox,SIGNAL(valueChanged(int)),ui->PietEditor,SLOT(setZoomFactor(int)));
    //connect(ui->actionZoom_In,&QAction::triggered,ui->PietEditor,IconEditor::incrementZoomFactor);
    connect(ui->actionZoom_In,SIGNAL(triggered(bool)),ui->PietEditor,SLOT(incrementZoomFactor()));
    connect(ui->actionZoom_Out,SIGNAL(triggered(bool)),ui->PietEditor,SLOT(decrementZoomFactor()));
    connect(ui->actionUndo,SIGNAL(triggered(bool)),ui->PietEditor,SLOT(undo()));
    connect(ui->action_Open,SIGNAL(triggered(bool)),this,SLOT(openImage()));
    connect(ui->PietEditor,SIGNAL(changedPenColor(const QColor &)),this,SLOT(setEditColor(const QColor &)));
    CB(0,0);CB(0,1);CB(0,2);CB(0,3);CB(0,4);CB(0,5);CB(0,6);
    CB(1,0);CB(1,1);CB(1,2);CB(1,3);CB(1,4);CB(1,5);CB(1,6);
    CB(2,0);CB(2,1);CB(2,2);CB(2,3);CB(2,4);CB(2,5);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::openImage(){
    auto fileName = QFileDialog::getOpenFileName(this,tr("Open Image"), "", tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
    //MSGBOX(fileName);
    ui->PietEditor->openImage(fileName);
}
void MainWindow::setEditColor(const QColor &c){
    QString strrgb = QString("background-color : rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue());
    ui->BUSER->setStyleSheet(strrgb+" \nalternate-"+strrgb+"\nselection-"+strrgb);
}
