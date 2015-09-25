#include "plotter.h"

Plotter::Plotter(const QScopedPointer<QFile>& data_file, QCustomPlot* plotter):
        status(Stopped), plotter(plotter), data_file(data_file), draw_data(new QCPDataMap),
        test("/home/nikita/test.test") {
    setupPlotter();
    // Since that point the plotter has the ownership of draw_data,
    // that's why don't worry about memory release - the plotter is
    // the responsible for that.
    this->plotter->graph(0)->setData(draw_data, false);
    test.open(QIODevice::Append | QIODevice::ReadOnly);
}

void Plotter::setupPlotter() const {
    plotter->setBackground(Qt::black);
    plotter->xAxis->setLabelColor(Qt::white);
    plotter->xAxis2->setLabelColor(Qt::white);
    plotter->yAxis->setLabelColor(Qt::white);
    plotter->xAxis->setLabel("degree, tag");
    plotter->xAxis2->setLabel("degree, grad");
    plotter->yAxis->setLabel("time, s");
    plotter->xAxis->setBasePen(QPen(Qt::white));
    plotter->xAxis->setTickPen(QPen(Qt::white));
    plotter->xAxis->setSubTickPen(QPen(Qt::white));
    plotter->xAxis2->setBasePen(QPen(Qt::white));
    plotter->xAxis2->setTickPen(QPen(Qt::white));
    plotter->xAxis2->setSubTickPen(QPen(Qt::white));
    plotter->yAxis->setBasePen(QPen(Qt::white));
    plotter->yAxis->setTickPen(QPen(Qt::white));
    plotter->yAxis->setSubTickPen(QPen(Qt::white));
    plotter->xAxis2->setVisible(true);
    plotter->xAxis->setRange(0, 1000);
    plotter->yAxis->setRange(0, 65535);
    plotter->addGraph();
    plotter->replot();
}

void Plotter::draw() const {  // TODO: faster?
    auto it = draw_data->begin();
    for (int i=0; i<draw_queue.size(); ++i) {
      it = draw_data->insert(it, i, QCPData(i, draw_queue[i]));
    }
    plotter->replot();
}

void Plotter::doWork() {
    ro_data_file.reset(new QFile(data_file->fileName()));
    ro_data_file->open(QIODevice::ReadOnly);    // TODO: if wrong?
    status = Active;
    while(isActive()) {
        this->thread()->msleep(kPlotDelay);
        readFromFile();
        draw();
    }
    ro_data_file->close();
    this->thread()->exit(0);
}

void Plotter::stop() {
    status = Stopped;
}

void Plotter::readFromFile() {
    u_int16_t data;
    while(ro_data_file->read(reinterpret_cast<char*>(&data), sizeof(data)) == -1);    //TODO: remove while
    test.write(reinterpret_cast<char*>(&data), 2);
    test.flush();
    if (draw_queue.size() == 1000) {
        draw_queue.pop_front();
    }
    draw_queue.push_back(20 + data);
}
