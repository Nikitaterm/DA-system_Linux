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
    void newSession(const QString& name, const QString& dataFileLocation, const QString& deviceFileName);

private slots:
    void dataFile_OnClick();
    void deviceFile_OnClick();
    void ok_OnClick();
    void cancel_OnClick();

private:
    Ui::NewSessionDialog *ui;
    QString data_file_name;
    QString device_file_name;
};

#endif // NEWSESSIONDIALOG_H
