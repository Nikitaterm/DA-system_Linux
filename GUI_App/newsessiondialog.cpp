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
}

NewSessionDialog::~NewSessionDialog()
{
    delete ui;
}

void NewSessionDialog::ok_OnClick() {
    if (dataFilename.isEmpty()) {
        MSG(this, ERROR, "Choose the data file name");
    } else if (ui->e_session_name->text().isEmpty()){
        MSG(this, ERROR, "Enter the session name");
    } else {
        emit newSession(ui->e_session_name->text(), dataFilename);
        this->close();
    }
}

void NewSessionDialog::dataFile_OnClick() {
    dataFilename = QFileDialog::getSaveFileName(0, "",
                               QDir::current().absolutePath(), ".raw");
}

void NewSessionDialog::cancel_OnClick() {
    this->close();
}
