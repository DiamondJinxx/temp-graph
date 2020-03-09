#include "tempgraph.h"
#include "ui_tempgraph.h"

TempGraph::TempGraph(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TempGraph)
{
    ui->setupUi(this);
    tmrupgraph = new QTimer();
    tmrX      = new QTimer(this);
    series    = new QLineSeries();
    chartView = new QChartView(chart);
    chart     = new QChart();
    axisX     = new QValueAxis();
    axisY     = new QValueAxis();

    tmrX->setInterval(1000);
    tmrupgraph->setInterval(500);
    /* соединяем серию, чарт и его представления */
    chart->addSeries(series);
    chart->legend()->hide();
    chart->setTitle("Температура с МК");
    axisX->setTitleText("Время,сек");
    axisX->setLabelFormat("%d");
    axisX->setTickCount(15);
    axisY->setTickCount(15);
    axisY->setTitleText("Температура, градусы С");
    axisY->setLabelFormat("%d");
    chart->addAxis(axisX,Qt::AlignBottom);
    series->attachAxis(axisX);// подключаем ось к значениям
    chart->addAxis(axisY,Qt::AlignLeft);
    series->attachAxis(axisY);// подключаем ось к значениям

    chartView->setChart(chart);

    ui->h_latout->addWidget(chartView);
    connect(tmrupgraph,SIGNAL(timeout()),this,SLOT(updateGraph()));
    tmrupgraph->start();
}

TempGraph::~TempGraph()
{
    delete ui;
}

void TempGraph::updateGraph()
{
    sec++;
//    temp += ui->box_data->value();
    //QLineSeries* series1 = new QLineSeries();
    //*series1<<QPoint(sec,temp);
    *series<<(QPoint(sec,temp));
    chart->removeSeries(series);
    chart->addSeries(series);
    //chartView->update(); //ему похер на эти обновления представления и самого чарта
    //chart->update();
    axisX->setMax(sec+5);
    axisY->setMax(50);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    ui->label->setText(QString::number(sec));
    ui->label_2->setText(QString::number(temp));
   //chartView->setChart(chart);
}
void TempGraph::on_btn_add_clicked()
{
   temp +=ui->box_data->value();
}

void TempGraph::on_btn_del_clicked()
{
   temp -=ui->box_data->value();

}

void TempGraph::on_btn_reset_clicked()
{

}
