#ifndef TEMPGRAPH_H
#define TEMPGRAPH_H

#include <QMainWindow>
#include <QtCharts/QChart>
#include <QtCharts/QtCharts>
#include <QTimer>
#include <QtDebug>

//using namespace QtCharts;
QT_BEGIN_NAMESPACE
namespace Ui { class TempGraph; }
QT_END_NAMESPACE

class TempGraph : public QMainWindow
{
    Q_OBJECT

    QChartView* chartView;
    QChart* chart;
    int temp = 0; // текущая температура графика
    int sec  = 0; // секунды графика
    QTimer* tmrX;
    QTimer* tmrupgraph;
    QLineSeries* series;
    QValueAxis *axesX;
    QValueAxis *axesY;

public:
    TempGraph(QWidget *parent = nullptr);
    ~TempGraph();

private slots:
    void updateGraph();
    void on_btn_add_clicked();

    void on_btn_del_clicked();

    void on_btn_reset_clicked();

private:
    Ui::TempGraph *ui;
};
#endif // TEMPGRAPH_H
