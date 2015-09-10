#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "messagehandler.h"

#include "qcustomplot.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->menuBar->setVisible(true);
    connect(ui->actionOpenSession, SIGNAL(triggered()), this, SLOT(openSession_OnClick()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(quit_OnClick()));
    connect(ui->actionNewSession, SIGNAL(triggered()), this, SLOT(newSession_OnClick()));
    new_session_dialog.reset(new NewSessionDialog(this));
    cur_session = sessions.end();
    connect(new_session_dialog.get(), SIGNAL(newSession(QString, QString)), this, SLOT(newSession(QString, QString)));
    connect(ui->actionCloseSession, SIGNAL(triggered()), this, SLOT(closeSession_OnClick()));
    ui->statusBar->addWidget(&status_session_name);
    ui->statusBar->addWidget(&status_data_file);
    updateCurrentSessionInfo();
    ui->actionCloseSession->setEnabled(false);
    ui->widget->setVisible(false);
    /*this->showFullScreen();
    ui->widget->resize(QApplication::desktop()->size().width(),
                       QApplication::desktop()->size().height() - 2*ui->statusBar->size().height());  //TODO: remove this workaround*/
    connect(ui->actionStart, SIGNAL(triggered()), this, SLOT(startSession_OnClick()));
    connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(stopSession_OnClick()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openSession_OnClick() {
    std::unique_ptr<Session> session(new Session(ui->widget));
    QString file_name = QFileDialog::getOpenFileName(0,"",
                                 QDir::current().absolutePath(),"*.ss");
    if (!file_name.isEmpty()) {
        QString err;
        if (!session->open(file_name, err)) {
            MSG(this, ERROR, err);
            return;
        }
        sessions.push_back(std::move(session));
        cur_session = sessions.end() - 1;
        updateCurrentSessionInfo();
        ui->actionOpenSession->setEnabled(false);   //TODO: create a method for this
        ui->actionNewSession->setEnabled(false);
        ui->actionCloseSession->setEnabled(true);
        ui->widget->setVisible(true);
    }
}

void MainWindow::quit_OnClick() {
    while(closeSession_OnClick());
    QApplication::quit();
}

void MainWindow::newSession_OnClick() {
    new_session_dialog->show();
}

void MainWindow::newSession(QString name, QString data_location) {
   std::unique_ptr<Session> session(new Session(ui->widget));
   QString err;
   if (!session->create(name, data_location, err)) {
       MSG(this, ERROR, err);
       return;
   }
   sessions.push_back(std::move(session));
   cur_session = sessions.end() - 1;
   updateCurrentSessionInfo();
   ui->actionOpenSession->setEnabled(false);
   ui->actionNewSession->setEnabled(false);
   ui->actionCloseSession->setEnabled(true);
   ui->widget->setVisible(true);
}

bool MainWindow::closeSession_OnClick() {
    if (cur_session != sessions.end()) {
        cur_session->get()->close();
        sessions.erase(cur_session);
        if (sessions.size() != 0) {
            cur_session = sessions.end() - 1;
        } else {
            cur_session = sessions.end();
        }
        updateCurrentSessionInfo();
        ui->actionOpenSession->setEnabled(true);
        ui->actionNewSession->setEnabled(true);
        ui->actionCloseSession->setEnabled(false);
        ui->widget->setVisible(false);
        return cur_session != sessions.end();
    }
    return false;
}

void MainWindow::updateCurrentSessionInfo() {
    if (cur_session != sessions.end()) {
        status_session_name.setText("Session: " + cur_session->get()->getSessionName());
        status_data_file.setText("Data file: " + cur_session->get()->getDataFileName());
    } else {
        status_session_name.setText("No open sessions");
        status_data_file.setText("");
    }
}

void MainWindow::startSession_OnClick() {
    QString err;
    if (!cur_session->get()->start(err)) {
        MSG(this, ERROR, err);
    }
}

void MainWindow::stopSession_OnClick() {
    QString err;
    if (!cur_session->get()->stop(err)) {
        MSG(this, ERROR, err);
    }
}
