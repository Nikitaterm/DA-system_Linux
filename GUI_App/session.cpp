#include "messagehandler.h"
#include "session.h"

#include <QFileDialog>
#include <QTextStream>

#define SESSION_FILE_DIR QDir::current().absolutePath() + "/sessions/"
#define EXT_POSTFIX ".ss"
#define DATA_FILE_POSTFIX ".raw"

bool Session::create(QString name, QString datafile_location, QString& err) {
    name_ = name;
    datafile_name_ = datafile_location + DATA_FILE_POSTFIX;
    session_file.reset(new QFile(SESSION_FILE_DIR + name_ + EXT_POSTFIX));
    data_file.reset(new QFile(datafile_name_));
    if (!session_file->open(QIODevice::WriteOnly)) {
        err = "Failed to create the session file!";
        return false;
    }
    if (!data_file->open(QIODevice::ReadWrite)) {
        err = "Failed to create the data file: " + datafile_name_;
        return false;
    }
    QTextStream out(session_file.data());
    out << name_ << "\n" << datafile_name_;
    session_file->close();
    status_ = Stopped;
    return true;
}

bool Session::open(QString session_file_name, QString& err) {
    if (!session_file_name.isEmpty()) {
        session_file.reset(new QFile(session_file_name));
        if (!session_file->open(QIODevice::ReadOnly)) {
            err = "Failed to open the session file!";
            return false;
        }
        QTextStream out(session_file.data());
        out >> name_ >> datafile_name_;
        data_file.reset(new QFile(datafile_name_));
        if (!data_file->open(QIODevice::ReadWrite | QIODevice::Append)) {
            err = "Failed to open a data file: " + datafile_name_ + "!";
            return false;
        }
        session_file->close();
        status_ = Stopped;
        return true;
    }
    return true;
}

void Session::close() {
    if (!isClosed()) {
        if (isActive()) {
            stop();
        }
        data_file->close();
        status_ = Closed;
    }
}

void Session::start() {
    status_ = Active;
}

void Session::stop() {
    status_ = Stopped;
}

bool Session::remove(bool erase_data, QString& err) {
    if (!isClosed()) {
        close();
    }
    if (!QFile::remove(session_file->fileName())) {
        err = "Failed to remove the session file";
        return false;
    }
    if (erase_data) {
        if (!QFile::remove(data_file->fileName())) {
            err = "Failed to remove the data file";
            return false;
        }
    }
    return true;
}
