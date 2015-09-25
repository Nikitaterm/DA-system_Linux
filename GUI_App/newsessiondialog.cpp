#include <QFileDialog>

#include "messagehandler.h"
#include "newsessiondialog.h"
#include "ui_newsessiondialog.h"

NewSessionDialog::NewSessionDialog(QWidget *parent):
        QDialog(parent),
        ui(new Ui::NewSessionDialog) {
    ui->setupUi(this);
    connect(ui->b_data_file, SIGNAL(clicked()), this, SLOT(dataFile_OnClick()));
    connect(ui->b_ok, SIGNAL(clicked()), this, SLOT(ok_OnClick()));
    connect(ui->b_cancel, SIGNAL(clicked()), this, SLOT(cancel_OnClick()));
    connect(ui->b_device_file, SIGNAL(clicked()), this, SLOT(deviceFile_OnClick()));
}

void NewSessionDialog::showEvent(QShowEvent*) {
    data_file_name = QString();
    device_file_name = QString();
    ui->e_session_name->setText(QString());
}

NewSessionDialog::~NewSessionDialog() {
    delete ui;
}

void NewSessionDialog::ok_OnClick() {
    if (data_file_name.isEmpty()) {
        MSG(this, ERROR, "Enter the data file name");
    } else if (ui->e_session_name->text().isEmpty()){
        MSG(this, ERROR, "Enter the session name");
    } else if (device_file_name.isEmpty()) {
        MSG(this, ERROR, "Choose the device");
    } else {
        emit newSession(ui->e_session_name->text(), data_file_name, device_file_name);
        this->close();
    }
}

void NewSessionDialog::dataFile_OnClick() {
    data_file_name = QFileDialog::getSaveFileName(0, "",
                         QDir::current().absolutePath(), ".raw");   // TODO: globals.h
}

void NewSessionDialog::deviceFile_OnClick() {
    device_file_name = QFileDialog::getOpenFileName(0, "",
                           "/dev/");    // TODO: globals.h
}

void NewSessionDialog::cancel_OnClick() {
    this->close();
}
