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

    int ports_count	  = 0;
    int temp = 0; // текущая температура графика
    int sec  = 0; // секунды графика
    bool gon = true; // контроль вывода графика
    bool port_is_available = false; // контроль соединения с выбранным портом.
    bool connect_once = false;
    bool req = false; // наличие запроса данных от мк
    QModelIndex cur_port_index;// индекс выбранного элемента в таблице
    QModelIndex con_port_index;// индекс элемента(порта), к которому уже подключены
    QSerialPort *port = nullptr;
    QStandardItemModel *modelOut; // модель вывода в бокс портов
    QChartView* chartView;
    QChart* chart;
    QTimer* tmr_up_ports;
    QTimer* tmrupgraph;
    QTimer* tmr_serialread;
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

    void serial_ports_out();

    void on_tblOut_clicked(const QModelIndex &index);


    void on_btn_connect_port_clicked();

    void on_btn_desconnect_port_clicked();

    void serialRead();
    void on_btn_query_clicked();

    void on_btn_stopquery_clicked();

private:
    Ui::TempGraph *ui;
};
#endif // TEMPGRAPH_H
