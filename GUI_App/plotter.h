#ifndef PLOTTER_H
#define PLOTTER_H

#include <stdint.h>
#include <deque>
#include <QFile>
#include <QObject>
#include <QScopedPointer>

#include "qcustomplot.h"

class Plotter : public QObject {
    Q_OBJECT
public:
    explicit Plotter(const QScopedPointer<QFile>& data_file, QCustomPlot* plotter);
    void setupPlotter() const;

public slots:
    void doWork();
    void stop();

private:
    enum Status {
        Active,
        Stopped
    };
    void draw();
    inline bool isActive() {
        return status_ == Active;
    }
    void readFromFile();

private:
    const QScopedPointer<QFile>& data_file_;
    QScopedPointer<QFile> ro_data_file;
    QCustomPlot* plotter_;
    Status status_;

    std::deque<uint16_t> draw_queue_;  // TODO: remove this
    QCPDataMap* draw_data_;

    QFile test;
};

#endif // PLOTTER_H
