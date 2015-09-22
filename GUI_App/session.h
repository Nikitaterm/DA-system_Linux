#ifndef SESSION_H
#define SESSION_H

#include <QFile>
#include <QString>
#include <QScopedPointer>
#include <QThread>

#include "datareader.h"
#include "qcustomplot.h"
#include "plotter.h"

class Session : public QObject {
    Q_OBJECT
public:
    explicit Session(QCustomPlot* plotter):
            status_(Closed),
            plotter_(data_file, plotter),
            data_reader_(data_file, dev_file_) {
        plotter_.moveToThread(&plotter_thread_);
        data_reader_.moveToThread(&data_read_thread_);
    }
    bool create(QString name, QString data_file_location, QString dev_file_location, QString& err);
    bool open(QString session_file_name, QString& err);
    bool start(QString& err);
    bool stop(QString& err);
    void close();
    bool remove(bool erase_data, QString& err);

    inline QString getSessionName() const {
        return name_;
    }

    inline QString getDataFileName() const {
        return data_file_name_;
    }

private:
    enum Status {
        Active,
        Stopped,
        Closed
    };
    inline bool isActive() const {
        return status_ == Active;
    }
    inline bool isClosed() const {
        return status_ == Closed;
    }

signals:
    void sessionThreadError(QString err);

private slots:
    void threadError(QString err);

private:
    QString name_;
    QString data_file_name_;    // TODO: remove?
    Status status_;

    QScopedPointer<QFile> session_file;
    QScopedPointer<QFile> data_file;
    QScopedPointer<QFile> dev_file_;

    QThread plotter_thread_;
    Plotter plotter_;

    QThread data_read_thread_;
    DataReader data_reader_;

};

#endif // SESSION_H
