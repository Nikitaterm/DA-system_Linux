#include "messagehandler.h"
#include "newsessiondialog.h"
#include "ui_newsessiondialog.h"

#include <QFileDialog>

NewSessionDialog::NewSessionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewSessionDialog)
{
    ui->setupUi(this);
    connect(ui->b_data_file, SIGNAL(clicked()), this, SLOT(dataFile_OnClick()));
    connect(ui->b_ok, SIGNAL(clicked()), this, SLOT(ok_OnClick()));
    connect(ui->b_cancel, SIGNAL(clicked()), this, SLOT(cancel_OnClick()));
    connect(ui->b_device_file, SIGNAL(clicked()), this, SLOT(deviceFile_onClick()));
}

NewSessionDialog::~NewSessionDialog()
{
    delete ui;
}

void NewSessionDialog::ok_OnClick() {
    if (data_file_name_.isEmpty()) {
        MSG(this, ERROR, "Enter the data file name");
    } else if (ui->e_session_name->text().isEmpty()){
        MSG(this, ERROR, "Enter the session name");
    } else if (device_file_name_.isEmpty()) {
        MSG(this, ERROR, "Choose the device");
    } else {
        emit newSession(ui->e_session_name->text(), data_file_name_, device_file_name_);
        this->close();
    }
}

void NewSessionDialog::dataFile_OnClick() {
    data_file_name_ = QFileDialog::getSaveFileName(0, "",
                               QDir::current().absolutePath(), ".raw");
}

void NewSessionDialog::deviceFile_onClick() {
    device_file_name_ = QFileDialog::getOpenFileName(0, "",
                               "/dev/");
}

void NewSessionDialog::cancel_OnClick() {
    this->close();
}
