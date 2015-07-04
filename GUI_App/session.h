#ifndef SESSION_H
#define SESSION_H

#include <QFile>
#include <QString>
#include <QScopedPointer>

enum Status {
    Active,
    Stopped,
    Closed
};

class Session
{
public:
    bool create(QString name, QString datafile_location, QWidget *self);
    bool open(QWidget *self);
    void start();
    void stop();
    void close();
    bool remove(bool erase_data, QWidget *self);

private:
    inline bool isActive() {
        return status_ == Active;
    }

    inline bool isClosed() {
        return status_ == Closed;
    }

private:
    QString name_;
    QString datafile_location_;
    Status status_;

    QScopedPointer<QFile> session_file;
    QScopedPointer<QFile> data_file;
};

#endif // SESSION_H
