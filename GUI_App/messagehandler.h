#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QMessageBox>
#include <QString>

enum Severity {
    INFO,
    WARNING,
    ERROR
};

#define MSG(self, severity, text) showMessage(self, severity, text)

static void showMessage(QWidget *self, Severity severity, const QString& text) {
    QMessageBox msg(self);
    msg.setText(text);
    msg.setStandardButtons(QMessageBox::Ok);
    switch (severity) {
        case INFO:    msg.setIcon(QMessageBox::Information); break;
        case WARNING: msg.setIcon(QMessageBox::Warning); break;
        case ERROR:   msg.setIcon(QMessageBox::Critical); break;
    }
    msg.exec();
}

#endif // MESSAGEHANDLER_H
