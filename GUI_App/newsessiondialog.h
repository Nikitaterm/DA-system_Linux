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
    void newSession(QString name, QString dataFileLocation);

private slots:
    void dataFile_OnClick();
    void ok_OnClick();
    void cancel_OnClick();

private:
    Ui::NewSessionDialog *ui;
    QString dataFilename;
};

#endif // NEWSESSIONDIALOG_H
