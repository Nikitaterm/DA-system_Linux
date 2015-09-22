#include <unistd.h>

#include "sys/file.h"
#include "plotter.h"

#define PLOT_DELAY 10

Plotter::Plotter(const QScopedPointer<QFile>& data_file, QCustomPlot* plotter):
        data_file_(data_file), plotter_(plotter), status_(Stopped), test("/home/nikita/test.test") {
    setupPlotter();
    draw_data_ = new QCPDataMap;
    // Since that point the plotter_ has the ownership of draw_data_,
    // that's why don't worry about memory release - the plotter_ is
    // the responsible for that.
    plotter_->graph(0)->setData(draw_data_, false);
    test.open(QIODevice::Append | QIODevice::ReadOnly);
}

void Plotter::setupPlotter() const {
    plotter_->setBackground(Qt::black);
    plotter_->xAxis->setLabelColor(Qt::white);
    plotter_->xAxis2->setLabelColor(Qt::white);
    plotter_->yAxis->setLabelColor(Qt::white);
    plotter_->xAxis->setLabel("degree, tag");
    plotter_->xAxis2->setLabel("degree, grad");
    plotter_->yAxis->setLabel("time, s");
    plotter_->xAxis->setBasePen(QPen(Qt::white));
    plotter_->xAxis->setTickPen(QPen(Qt::white));
    plotter_->xAxis->setSubTickPen(QPen(Qt::white));
    plotter_->xAxis2->setBasePen(QPen(Qt::white));
    plotter_->xAxis2->setTickPen(QPen(Qt::white));
    plotter_->xAxis2->setSubTickPen(QPen(Qt::white));
    plotter_->yAxis->setBasePen(QPen(Qt::white));
    plotter_->yAxis->setTickPen(QPen(Qt::white));
    plotter_->yAxis->setSubTickPen(QPen(Qt::white));
    plotter_->xAxis2->setVisible(true);
    plotter_->xAxis->setRange(0, 1000);
    plotter_->yAxis->setRange(0, 65535);
    plotter_->addGraph();
    plotter_->replot();
}

void Plotter::draw() {
    auto it = draw_data_->begin();
    for (int i=0; i<draw_queue_.size(); ++i) {
      it = draw_data_->insert(it, i, QCPData(i, draw_queue_[i]));
    }
    plotter_->replot();
}

void Plotter::doWork() {
    ro_data_file.reset(new QFile(data_file_->fileName()));
    ro_data_file->open(QIODevice::ReadOnly);
    status_ = Active;
    while(isActive()) {
        this->thread()->msleep(PLOT_DELAY);
        readFromFile();
        draw();
    }
    ro_data_file->close();
    this->thread()->exit(0);
}

void Plotter::stop() {
    status_ = Stopped;
}

void Plotter::readFromFile() {
    u_int16_t data;
    //flock(data_file_->handle(), LOCK_EX);
    while(read(ro_data_file->handle(), reinterpret_cast<char*>(&data), sizeof(data)) == -1);    //TODO: remove while
    //flock(data_file_->handle(), LOCK_UN);
    //test.write(reinterpret_cast<char*>(&data), 2);
    plotter_->replot();
    if (draw_queue_.size() == 1000) {
        draw_queue_.pop_front();
    }
    draw_queue_.push_back(20 + data);
}
