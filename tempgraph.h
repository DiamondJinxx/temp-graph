#ifndef TEMPGRAPH_H
#define TEMPGRAPH_H

#include <QMainWindow>
#include <QtCharts/QChart>
#include <QtCharts/QtCharts>
#include <QTimer>
#include <QtDebug>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QtSerialPort>
#include <QString>

//using namespace QtCharts;
QT_BEGIN_NAMESPACE
namespace Ui { class TempGraph; }
QT_END_NAMESPACE

class TempGraph : public QMainWindow
{
    Q_OBJECT

    int ports_count	 = 0;
    int temp = 0; // текущая температура графика
    int sec  = 0; // секунды графика
    bool gon = true; // контроль вывода графика
    bool port_is_available = false; // контроль соединения с выбранным портом.
    QStandardItemModel *modelOut; // модель вывода в бокс портов
    QChartView* chartView;
    QChart* chart;
    QTimer* tmr_up_ports;
    QTimer* tmrupgraph;
    QLineSeries* series;
    QValueAxis *axesX;
    QValueAxis *axesY;
  //  QMessageBox *msg;

public:
    TempGraph(QWidget *parent = nullptr);
    ~TempGraph();

private slots:
    void updateGraph();
    void on_btn_add_clicked();

    void on_btn_del_clicked();

    void on_btn_reset_clicked();

    void on_btn_begin_clicked();

    void serialPortInfo();

    void on_btn_portInfo_clicked();

    void ports_out();
private:
    Ui::TempGraph *ui;
};
#endif // TEMPGRAPH_H
