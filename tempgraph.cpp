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
    if(gon == true)
    {
        gon = false;
        series->clear();
        tmrupgraph->stop();
        sec = 0;
    }
    else
    {
        QMessageBox::information(this,"Предупреждение","График уже не отображается");
    }
}

void TempGraph::on_btn_begin_clicked()
{
    if(gon == true)
    {
        QMessageBox::information(this,"Предупреждение","График уже  отображается");
    }
    else
    {
        *series<<QPoint(0,0);
        tmrupgraph->start();
        gon = true;
    }
}

void TempGraph::serialPortInfo()
{
    /*
     * функция для вывода информации о доступных com-портах
     * для вывода тестово используем диалоговое окно
    */
    QMessageBox * msg;
    QString info;
    //const QSerialPortInfo port;
    info.append("Количество доступных портов:\t"+QString::number(QSerialPortInfo::availablePorts().length())+"\n\n");
    qDebug()<<QSerialPortInfo::availablePorts().length()<<"\n";
    qDebug()<<"Описание каждого пора в отдельности:\n";
    /* константная ссылка - дает доступ к элементу без возможности его изменение(только для чтения) */
    foreach (const QSerialPortInfo& portInfo, QSerialPortInfo::availablePorts() )
    {
     //  qDebug()<<"Описание: "<<portInfo.description()<<"\n";
     //  qDebug()<<"Имеется vendor id?: "<<portInfo.hasVendorIdentifier()<<"\n";
     //qDebug()<<"Сам vendor: "<<portInfo.vendorIdentifier()<<"\n";
     //  qDebug()<<"Имеется Devics id?: "<<portInfo.hasProductIdentifier()<<"\n";
     //  qDebug()<<"Device Id: "<<portInfo.productIdentifier()<<"\n";
       info.append("Описание порта:\t"+portInfo.description()+"\n");
       QString hasVendor = portInfo.hasVendorIdentifier() ? "имеется":"не имеется";
       QString hasDevice = portInfo.hasProductIdentifier() ? "имеется":"не имееется";
       info.append("Наличие Vendor ID:\t"+hasVendor+"\n");
       info.append("Vendor ID:\t"+QString::number(portInfo.vendorIdentifier())+"\n");
       info.append("Наличие Device ID:\t"+hasDevice+"\n");
       info.append("QVendor ID:\t"+QString::number(portInfo.productIdentifier())+"\n");
       info.append("\n");
    }
    msg = new QMessageBox();
    msg->setText(info);
    msg->setWindowTitle("Информация о доступных портах");
    msg->show();
}

void TempGraph::on_btn_portInfo_clicked()
{
   serialPortInfo();
}
