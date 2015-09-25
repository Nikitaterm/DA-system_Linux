#ifndef SESSION_H
#define SESSION_H

#include <QFile>
#include <QString>
#include <QScopedPointer>
#include <QThread>

#include "datareader.h"
#include "globals.h"
#include "qcustomplot.h"
#include "plotter.h"

class Session : public QObject {
    Q_OBJECT
public:
    explicit Session(QCustomPlot* plotter);
    bool create(const QString& name, const QString& datafile_location, const QString& devfile_location,
                QString& err);
    bool open(const QString& session_file_name, QString& err);
    bool close(QString& err);
    bool start(QString& err);
    bool stop(QString& err);
    bool remove(bool erase_data, QString& err);

    inline QString getSessionName() const {
        return name;
    }
    inline QString getDataFileName() const {
        if (data_file != nullptr) {
            return data_file->fileName();
        }
        return QString();
    }

signals:
    void sessionThreadError(const QString& err);

private slots:
    void threadError(const QString& err);

private:
    enum Status {
        Active,
        Stopped,
        Closed
    };
    inline bool isActive() const {
        return status == Active;
    }
    inline bool isClosed() const {
        return status == Closed;
    }

private:
    QString name;
    Status status;
    QScopedPointer<QFile> session_file;     // TODO: use unique_ptr instead
    QScopedPointer<QFile> data_file;
    QScopedPointer<QFile> dev_file;
    QThread plotter_thread;
    Plotter plotter;
    QThread data_read_thread;
    DataReader data_reader;
};

#endif // SESSION_H
