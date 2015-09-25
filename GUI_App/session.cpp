#include <QDataStream>
#include <QFileDialog>

#include "messagehandler.h"
#include "session.h"

Session::Session(QCustomPlot* plotter_):
        status(Closed), plotter(data_file, plotter_),
        data_reader(data_file, dev_file) {
    plotter.moveToThread(&plotter_thread);
    data_reader.moveToThread(&data_read_thread);
}

bool Session::create(const QString& name, const QString& datafile_location, const QString& devfile_location,
                     QString& err) {
    this->name = name;
    QString datafile_location_ = datafile_location + DATA_FILE_EXT;
    session_file.reset(new QFile(SESSION_FILE_DIR + this->name + SESSION_FILE_EXT));
    data_file.reset(new QFile(datafile_location_));
    dev_file.reset(new QFile(devfile_location));
    if (!session_file->open(QIODevice::WriteOnly)) {
        err = "Failed to create the session file!";
        return false;
    }
    if (!data_file->open(QIODevice::Append | QIODevice::ReadOnly)) {
        err = "Failed to create the data file: " + datafile_location_;
        return false;
    }
    QDataStream out(session_file.data());
    out << this->name << datafile_location_ << devfile_location;
    session_file->close();
    status = Stopped;
    return true;
}

bool Session::open(const QString& sessionfile_name, QString& err) {
    if (!sessionfile_name.isEmpty()) {
        session_file.reset(new QFile(sessionfile_name));
        if (!session_file->open(QIODevice::ReadOnly)) {
            err = "Failed to open the session file!";
            return false;
        }
        QDataStream out(session_file.data());
        QString dev_filelocation;
        QString data_file_name;
        out >> name >> data_file_name >> dev_filelocation;
        data_file.reset(new QFile(data_file_name));
        dev_file.reset(new QFile(dev_filelocation));
        if (!data_file->open(QIODevice::Append | QIODevice::ReadOnly)) {
            err = "Failed to open a data file: " + data_file_name + "!";
            return false;
        }
        session_file->close();
        status = Stopped;
        return true;
    } else {
        err = "Session file name is empty!";
        return false;
    }
}

bool Session::close(QString& err) {
    if (!isClosed()) {
        if (isActive()) {
            if (!stop(err)) {
                return false;
            }
        }
        data_file->close();
        status = Closed;
        return true;
    } else {
        err = "Session is already closed!";
        return false;
    }
}

bool Session::start(QString& err) {
    if (!isClosed() && !isActive()) {
        connect(&data_read_thread, SIGNAL(started()), &data_reader, SLOT(doWork()));
        connect(&data_reader, SIGNAL(error(const QString&)), this, SLOT(threadError(const QString&)));
        connect(&plotter_thread, SIGNAL(started()), &plotter, SLOT(doWork()));
        data_reader.thread()->start();
        plotter_thread.start();
        status = Active;
        return true;
    } else {
        err = "Can't start the session!";
        return false;
    }
}

bool Session::stop(QString& err) {
    if (isActive()) {
        disconnect(&data_read_thread, SIGNAL(started()), &data_reader, SLOT(doWork()));
        disconnect(&data_reader, SIGNAL(error(const QString&)), this, SLOT(threadError(const QString&)));
        disconnect(&plotter_thread, SIGNAL(started()), &plotter, SLOT(doWork()));
        if (data_reader.thread()->isRunning()) {
            data_reader.stop();
            data_reader.thread()->wait();
        }
        if (plotter.thread()->isRunning()) {
            plotter.stop();
            plotter.thread()->wait();
        }
        status = Stopped;
        return true;
    } else {
        err = "Can't stop a not active session";
        return false;
    }
}

bool Session::remove(bool erase_data, QString& err) {
    if (!isClosed()) {
        if (!close(err)) {
            return false;
        }
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

void Session::threadError(const QString& err) {
    QString i_err;
    stop(i_err);
    emit sessionThreadError(err + " " + i_err);
}
