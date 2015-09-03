#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <vector>

#include <QLabel>
#include <QMainWindow>
#include <QString>
#include <QVector>

#include "newsessiondialog.h"
#include "session.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openSession_OnClick();
    void quit_OnClick();
    void newSession_OnClick();
    void newSession(QString name, QString data_location);
    bool closeSession_OnClick();  // Return true if there is at least one opened session

private:
    void updateCurrentSessionInfo();

private:
    Ui::MainWindow *ui;
    std::vector<std::unique_ptr<Session>> sessions;
    std::unique_ptr<NewSessionDialog> new_session_dialog;
    std::vector<std::unique_ptr<Session>>::iterator cur_session;

    QLabel status_session_name;
    QLabel status_data_file;
};

#endif // MAINWINDOW_H
