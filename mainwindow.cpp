#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //setAttribute(Qt::WindowStaysOnBottomHint, 1);
    setWindowFlag(Qt::WindowStaysOnBottomHint, 1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

