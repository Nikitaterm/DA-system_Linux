#include "messagehandler.h"
#include "session.h"

#include <QFileDialog>
#include <QTextStream>

#define SESSION_FILE_DIR QDir::current().absolutePath() + "/sessions/"
#define EXT_POSTFIX ".ss"
#define DATA_FILE_POSTFIX ".raw"

bool Session::create(QString name, QString data_file_location, QString dev_file_location, QString& err) {
    name_ = name;
    data_file_location = data_file_location + DATA_FILE_POSTFIX;
    session_file.reset(new QFile(SESSION_FILE_DIR + name_ + EXT_POSTFIX));
    data_file.reset(new QFile(data_file_location));
    dev_file_.reset(new QFile(dev_file_location));
    if (!session_file->open(QIODevice::WriteOnly)) {
        err = "Failed to create the session file!";
        return false;
    }
    if (!data_file->open(QIODevice::ReadWrite)) {
        err = "Failed to create the data file: " + data_file_location;
        return false;
    }
    QTextStream out(session_file.data());   // TODO: use QDataStream instead
    out << name_ << "\n" << data_file_location << "\n" << dev_file_location;
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
        QString dev_file_location;
        out >> name_ >> data_file_name_ >> dev_file_location;
        data_file.reset(new QFile(data_file_name_));
        dev_file_.reset(new QFile(dev_file_location));
        if (!data_file->open(QIODevice::ReadWrite | QIODevice::Append)) {
            err = "Failed to open a data file: " + data_file_name_ + "!";
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
            QString tmp;    // TODO: avoid tmp
            stop(tmp);
        }
        data_file->close();
        status_ = Closed;
    }
}

bool Session::start(QString& err) {
    if (!isClosed() && !isActive()) {
        connect(&data_read_thread_, SIGNAL(started()), &data_reader_, SLOT(doWork()));
        connect(&data_reader_, SIGNAL(error(QString)), this, SLOT(threadError(QString)));
        connect(&plotter_thread_, SIGNAL(started()), &plotter_, SLOT(doWork()));
        data_read_thread_.start();  // TODO: change to data_reader_.thread()->start()
        plotter_thread_.start();
        status_ = Active;
        return true;
    } else {
        err = "Can't start the session!";
        return false;
    }
}

bool Session::stop(QString& err) {
    if (isActive()) {
        // TODO: resolve this workaround
        disconnect(&data_read_thread_, SIGNAL(started()), &data_reader_, SLOT(doWork()));
        disconnect(&plotter_thread_, SIGNAL(started()), &plotter_, SLOT(doWork()));
        if (data_reader_.thread()->isRunning()) {
            data_reader_.stop();
            data_reader_.thread()->wait();
        }
        if (plotter_.thread()->isRunning()) {
            plotter_.stop();
            plotter_.thread()->wait();
        }
        status_ = Stopped;
        return true;
    } else {
        err = "Can't stop a not active session";
        return false;
    }
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

void Session::threadError(QString err) {
    QString tmp;
    stop(tmp);
    emit sessionThreadError(err);   // TODO: what about an error during the stop?
}
