#include "messagehandler.h"
#include "session.h"

#include <QFileDialog>
#include <QTextStream>

#define EXT_POSTFIX ".ss"

bool Session::create(QString name, QString datafile_location, QWidget *self) {
    name_ = name;
    datafile_location_ = datafile_location;
    status_ = Stopped;
    session_file.reset(new QFile(name_ + EXT_POSTFIX));
    data_file.reset(new QFile(datafile_location_));
    if (!session_file->open(QIODevice::WriteOnly)) {
        MSG(self, ERROR, "Failed to create a session file!");
        return false;
    }
    if (!data_file->open(QIODevice::ReadWrite)) {
        MSG(self, ERROR, "Failed to create a data file: "
            + datafile_location_ + "!");
        return false;
    }
    QTextStream out(session_file.data());
    out << name_ << "\n" << datafile_location_;
    session_file->close();
    return true;
}

bool Session::open(QWidget *self) {
    session_file.reset(new QFile(QFileDialog::getOpenFileName(self,"",
                                 QDir::current().absolutePath(),"*.ss")));
    if (!session_file->open(QIODevice::ReadOnly)) {
        MSG(self, ERROR, "Failed to open the session file!"); //TODO: keep in mind wrong files
        return false;
    }
    QTextStream out(session_file.data());
    out >> name_ >> datafile_location_;
    data_file.reset(new QFile(datafile_location_));
    if (!data_file->open(QIODevice::ReadWrite | QIODevice::Append)) {
        MSG(self, ERROR, "Failed to open a data file: "
            + datafile_location_ + "!");
        return false;
    }
    status_ = Stopped;
    session_file->close();
    return true;
}

void Session::close() {
    if (isActive()) {
        stop();
    }
    data_file->close();
    status_ = Closed;
}

void Session::start() {
    status_ = Active;
}

void Session::stop() {
    status_ = Stopped;
}

bool Session::remove(bool erase_data, QWidget *self) {
    if (!isClosed()) {
        close();
    }
    if (!QFile::remove(session_file->fileName())) {
        MSG(self, ERROR, "Failed to remove the session file");
        return false;
    }
    if (erase_data) {
        if (!QFile::remove(data_file->fileName())) {
            MSG(self, ERROR, "Failed to remove the data file");
            return false;
        }
    }
    return true;
}
