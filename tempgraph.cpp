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
    axesX     = new QValueAxis();
    axesY     = new QValueAxis();

    tmrX->setInterval(1000);
    tmrupgraph->setInterval(500);
    /* соединяем серию, чарт и его представления */
    chart->addSeries(series);
    chart->legend()->hide();
    chart->setTitle("Температура с МК");
    axesX->setTitleText("Время,сек");
    axesX->setLabelFormat("%0.2f");
    axesX->setTickCount(15);
    axesY->setTickCount(21);
    axesY->setTitleText("Температура, градусы С");
    axesY->setLabelFormat("%0.2f");
    chart->addAxis(axesX,Qt::AlignBottom);
    series->attachAxis(axesX);// подключаем ось к значениям
    chart->addAxis(axesY,Qt::AlignLeft);
    series->attachAxis(axesY);// подключаем ось к значениям
    *series<<QPoint(0,0);// добавляем начальную току, чтобы рисовалось от начала координат
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
    /*
     *  вызов метода repaint у представления должен вызываться автоматически
     *  никакого использования нового графика с дополнительным выделением памяти
     *  добавление в серию точки и очередная ее отрисовка
    */
    sec++;
    *series<<(QPoint(sec,temp));
    /*  оси чарта хранятся объектом класса QList, метод back() - аналог метода last()
     *	а так как чарт может иметь более одной оси, то таким образом мы общаемся к нашей оси(она единственная)
     * и сдвигаем ее.
    */
    chart->axes(Qt::Vertical).back()->setRange(-30,+30);
    chart->axes(Qt::Horizontal).back()->setRange(0,sec+10);
    /*
    chart->removeSeries(series);
    chart->addSeries(series);
    axesX->setMax(sec+5);
    axesY->setMax(50);
    series->attachAxis(axesX);
    series->attachAxis(axesY);
    */
    ui->lbl_time->setText(QString::number(sec));
    ui->lbl_temp->setText(QString::number(temp));
   // qDebug() << series->pointsVector();
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
    series->clear();
    tmrupgraph->stop();
    sec = 0;
}
