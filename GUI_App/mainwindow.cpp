#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include "newsessiondialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->menuBar->setVisible(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
