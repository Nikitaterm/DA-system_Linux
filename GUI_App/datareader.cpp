#include <QThread>
#include "sys/file.h"
#include <unistd.h>
#include "datareader.h"

DataReader::DataReader(const QScopedPointer<QFile>& data_file, const QScopedPointer<QFile>& device_file):
        data_file_(data_file), device_file_(device_file) {
}

void DataReader::doWork() {
    if (openDevice()) {
        status_ = Active;
        while(isActive()) {
            if (!getData()) {
                status_ = Stopped;
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
    status_ = Stopped;
}

bool DataReader::openDevice() {
    if (!device_file_->open(QIODevice::ReadOnly)) {
        return false;
    }
    return true;
}

void DataReader::closeDevice() {
    device_file_->close();
}

bool DataReader::getData() {    // TODO: maybe it's possible to do it faster?
    u_int16_t buff;
    if (device_file_->read(reinterpret_cast<char*>(&buff), sizeof(buff)) != sizeof(buff)) {  // TODO: why not to get 4 bytes?
        return false;
    }
    if (write(data_file_->handle(), reinterpret_cast<char*>(&buff), sizeof(buff)) != sizeof(buff)) {
        return false;
    }
    return true;
}
