#ifndef NEWSESSIONDIALOG_H
#define NEWSESSIONDIALOG_H

#include <QDialog>

namespace Ui {
class NewSessionDialog;
}

class NewSessionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewSessionDialog(QWidget *parent = 0);
    ~NewSessionDialog();

signals:
    void newSession(QString name, QString dataFileLocation, QString deviceFileName);    //TODO: refactor function names

private slots:
    void dataFile_OnClick();
    void deviceFile_onClick();
    void ok_OnClick();
    void cancel_OnClick();

private:
    Ui::NewSessionDialog *ui;
    QString data_file_name_;
    QString device_file_name_;
};

#endif // NEWSESSIONDIALOG_H
