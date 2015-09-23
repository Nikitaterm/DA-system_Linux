#ifndef DATAREADER_H
#define DATAREADER_H

#include <QFile>
#include <QScopedPointer>
#include <QString>

class DataReader : public QObject
{
    Q_OBJECT
public:
    explicit DataReader(const QScopedPointer<QFile>& data_file, const QScopedPointer<QFile>& device_file);

signals:
    void error(const QString& err);

public slots:
    void doWork();
    void stop();

private:
    enum Status {
        Active,
        Stopped
    };
    inline bool isActive() const {
        return status == Active;
    }
    bool openDevice() const;
    bool getData() const;
    void closeDevice() const;

private:
    Status status;
    const QScopedPointer<QFile>& data_file;
    const QScopedPointer<QFile>& device_file;
};

#endif // DATAREADER_H
