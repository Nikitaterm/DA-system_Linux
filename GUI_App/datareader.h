#ifndef DATAREADER_H
#define DATAREADER_H

#include <QFile>
#include <QObject>
#include <QScopedPointer>
#include <QString>

class DataReader : public QObject
{
    Q_OBJECT
public:
    explicit DataReader(const QScopedPointer<QFile>& data_file, const QScopedPointer<QFile>& device_file);

signals:
    void error(QString err);

public slots:
    void doWork();
    void stop();

private:
    enum Status {
        Active,
        Stopped
    };
    inline bool isActive() {
        return status_ == Active;
    }
    bool openDevice();
    bool getData();
    void closeDevice();

private:
    Status status_;

    const QScopedPointer<QFile>& data_file_;
    const QScopedPointer<QFile>& device_file_;

};

#endif // DATAREADER_H
