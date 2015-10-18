#include <QThread>

#include "datareader.h"

DataReader::DataReader(const QScopedPointer<QFile>& data_file, const QScopedPointer<QFile>& device_file):
        data_file(data_file), device_file(device_file) {
}

void DataReader::doWork() {
    if (openDevice()) {
        status = Active;
        while(isActive()) {
            if (!getData()) {
                status = Stopped;
                emit error("can't get data from the device file.");
            }
        }
        closeDevice();
    } else {
        emit error("can't open the device file.");
    }
    this->thread()->exit(0);
}

void DataReader::stop() {
    status = Stopped;
}

bool DataReader::openDevice() const {
    if (!device_file->open(QIODevice::ReadOnly)) {
        return false;
    }
    return true;
}

void DataReader::closeDevice() const {
    device_file->close();
}

bool DataReader::getData() const {    // TODO: maybe it's possible to do it faster?
    u_int16_t buff = 0;
    qint64 ret = 0;
    do {
        ret = device_file->read(reinterpret_cast<char*>(&buff), sizeof(buff)); // TODO: why not to get 4 bytes?
        if (ret == 0) {
            thread()->msleep(10);   // TODO: remove sleep and while?
        } else if (ret != sizeof(buff)) {
            return false;
        }
    } while(ret == 0 && isActive());
    if (isActive()) {
        if (data_file->write(reinterpret_cast<char*>(&buff), sizeof(buff)) != sizeof(buff)) {
            return false;
        }
        data_file->flush();
    }
    return true;
}
