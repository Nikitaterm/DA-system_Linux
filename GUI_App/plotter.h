#ifndef PLOTTER_H
#define PLOTTER_H

#include <deque>

#include <QFile>
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
    void readFromFile();
    void draw() const;
    inline bool isActive() const {
        return status == Active;
    }

private:
    Status status;
    QCustomPlot* plotter;
    const QScopedPointer<QFile>& data_file;
    QScopedPointer<QFile> ro_data_file;
    std::deque<uint16_t> draw_queue;
    QCPDataMap* const draw_data;
    QFile test; // TODO: remove.
};

#endif // PLOTTER_H
