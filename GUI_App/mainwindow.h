#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <vector>

#include <QLabel>
#include <QMainWindow>
#include <QString>

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
    void newSession_OnClick();
    void openSession_OnClick();
    bool closeSession_OnClick();  // Return true if there is at least one opened session
    void startSession_OnClick() const;
    void stopSession_OnClick() const;
    void quit_OnClick();

    void newSession(const QString& name, const QString& data_location, const QString& dev_file_location);
    void sessionThreadError(const QString& err) const;

private:
    void closeAllSessions();
    void updateCurrentSessionInfo();

    void updateGUIAfterSessionOpen() const;
    void updateGUIAfterSessionClose() const;

private:
    Ui::MainWindow *ui;
    const std::unique_ptr<NewSessionDialog> new_session_dialog;
    std::vector<std::unique_ptr<Session>> sessions;
    std::vector<std::unique_ptr<Session>>::iterator cur_session;
    QLabel status_session_name;
    QLabel status_data_file;
};

#endif // MAINWINDOW_H
