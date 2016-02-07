#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iconeditor.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    tabifyDockWidget(ui->DockProjectFileTree,ui->DockStack);
    //tabifyDockWidget(ui->DockInput,ui->DockOutput);
    //tabifyDockWidget(ui->DockInput,ui->DockStatus);
    //splitDockWidget(ui->DockStatus,ui->DockInput,Qt::Vertical);
    //splitDockWidget(ui->DockInput,ui->DockOutput,Qt::Horizontal);
    //connect(ui->sizespinBox,SIGNAL(valueChanged(int)),ui->PietEditor,SLOT(setZoomFactor(int)));
    connect(ui->actionZoom_In,SIGNAL(triggered(bool)),ui->PietEditor,SLOT(incrementZoomFactor()));
    connect(ui->actionZoom_Out,SIGNAL(triggered(bool)),ui->PietEditor,SLOT(decrementZoomFactor()));

}

MainWindow::~MainWindow()
{

    delete ui;
}
