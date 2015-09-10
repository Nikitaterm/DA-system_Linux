#include "plotter.h"

#define PLOT_DELAY 10

Plotter::Plotter(const QScopedPointer<QFile>& data_file_, QCustomPlot* plotter):
    data_file(data_file_), plotter_(plotter), status_(Stopped) {
    setupPlotter();
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
    plotter_->xAxis->setRange(0, 100);
    plotter_->yAxis->setRange(0, 100);
    plotter_->addGraph();
    plotter_->replot();
}

void Plotter::draw() {
    QVector<double> x(draw_queue_.size()), y(draw_queue_.size());
    for (int i=0; i<draw_queue_.size(); ++i)
    {
      x[i] = i;
      y[i] = draw_queue_[i];
    }
    plotter_->graph(0)->clearData();
    plotter_->graph(0)->addData(x, y);
    plotter_->replot();
}

void Plotter::start() {
    status_ = Active;
    while(isActive()) {
        this->thread()->msleep(PLOT_DELAY);
        readFromFile();
        draw();
    }
    emit finished();
}

void Plotter::stop() {
    status_ = Stopped;
}

void Plotter::readFromFile() {
    static int it = 0;
    /*QString data;
    data = data_file->readLine();
    plotter_->xAxis->setLabel(data);
    plotter_->replot();*/
    uint16_t data = it++;
    if (it == 20) {
        it = 0;
    }
    if (draw_queue_.size() == 100) {
        draw_queue_.pop_front();
    }
    draw_queue_.push_back(20 + data);
}
