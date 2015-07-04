#include "newsessiondialog.h"
#include "ui_newsessiondialog.h"
#include "messagehandler.h"
#include "session.h"

NewSessionDialog::NewSessionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewSessionDialog)
{
    ui->setupUi(this);
    connect(ui->b_create, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(ui->b_close, SIGNAL(clicked()), this, SLOT(close()));
}

NewSessionDialog::~NewSessionDialog()
{
    delete ui;
}

void NewSessionDialog::onOk() {

}
