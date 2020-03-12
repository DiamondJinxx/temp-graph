#include "tempgraph.h"
#include "ui_tempgraph.h"
//#define SEC (float)0.5 // в зависимости от интервала таймера графика нужно менять эту переменную для вывода секунд на оси графика

TempGraph::TempGraph(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TempGraph)
{
    ui->setupUi(this);
    /* блок инициализации  */
    tmr_up_ports      = new QTimer(this);
    tmrupgraph 		  = new QTimer();
    series    		  = new QLineSeries();
    chartView 		  = new QChartView(chart);
    chart     		  = new QChart();
    axesX     		  = new QValueAxis();
    axesY     		  = new QValueAxis();
    modelOut  		  = new QStandardItemModel(1, 3, this);
    modelOut->removeRows(0,1);
    modelOut->setHeaderData(0,Qt::Horizontal,"Описание");
    modelOut->setHeaderData(1,Qt::Horizontal,"Vendor");
    modelOut->setHeaderData(2,Qt::Horizontal,"Device");

    /* блок начальных установок */
    tmr_up_ports->setInterval(100);
    tmrupgraph->setInterval(1000);
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
    ui->tblOut->setModel(modelOut);//связываем модель и представление
    ui->tblOut->setEditTriggers(QTableView::NoEditTriggers);//запрет редактирования,можно сделать и через форму, но кто туда смотрит?
    ui->tblOut->setSelectionBehavior(QAbstractItemView::SelectRows); // установка выделение только строк в таблице


    ui->graph_latout->addWidget(chartView);
    connect(tmrupgraph,SIGNAL(timeout()),this,SLOT(updateGraph()));
    connect(tmr_up_ports,SIGNAL(timeout()),this,SLOT(serial_ports_out()));

    tmrupgraph->start();
    tmr_up_ports->start();
    qDebug()<<cport_index.isValid();
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
        *series<<QPoint(0,temp); // рисовала сразу со зачение, которое передает мк
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
    //поимка числа портов - для поимки пустой строки в представлении
    //qDebug()<<QSerialPortInfo::availablePorts().length()<<"\n";
    //qDebug()<<"Описание каждого пора в отдельности:\n";
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
       //info.append(portInfo.portName()+"\n");
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
   qDebug()<<port;
}

void TempGraph::serial_ports_out()
{
    if(ports_count != QSerialPortInfo::availablePorts().length())
    {
        int row = 0;
        QModelIndex dsc, vndr, dev;
        QString description, vendor, device;
        modelOut->removeRows(0,modelOut->rowCount());
       // modelOut->insertRows(0,1); // нужно, иначе не хватает строк для портов
        ports_count = QSerialPortInfo::availablePorts().length();
        foreach(const QSerialPortInfo& portInfo,QSerialPortInfo::availablePorts())
        {
            //целяем нужную информацию
             description = portInfo.description();
            // qDebug() << description.length();
             if(description.length()== 0)
                 description = "Системный";
             vendor      = QString::number(portInfo.vendorIdentifier());
             device      = QString::number(portInfo.productIdentifier());
            //по индексам в модели цепляем нужные элементы
             row = modelOut->rowCount();
             modelOut->insertRows(row,1);
             //отладка числа строк для удаления пустой строки
           //  qDebug() << "число строк: "<<row;
            // qDebug() << modelOut->rowCount();

             dsc = modelOut->index(row , 0);
             vndr = modelOut->index(row , 1);
             dev = modelOut->index(row , 2);
             //отладка индексирования
             //qDebug() << "indexs: " << dsc << vndr << dev;

        //добавляем информацию в модель для вывода
           //  modelOut->insertRows(row,1);
             modelOut->setData(dsc,description);
             modelOut->setData(vndr,vendor);
             modelOut->setData(dev,device);

             //отладка индексирования
             //qDebug() << description << vendor << device<<"\n";
        }
    }
}

// получаем индекс выбранного СТОЛБЦА, потому что выделяется в таблице СТОЛБЕЦ!

void TempGraph::on_tblOut_clicked(const QModelIndex &index)
{
    //икали зацепку для процерки совершения выбора элемента в таблице
    qDebug() << (cport_index.isValid());
    cport_index = ui->tblOut->currentIndex();
    qDebug()<<cport_index.isValid();
    //попытки выцепить данные напрямую через получаемый индекс
    //qDebug()<<cport_index;
    //qDebug()<<cport_index.row();
    //qDebug()<<modelOut->index(cport_index.row(),1).data().toInt(); // позволяет выцепить данные по индексу модели и сразу преобразовать к целому типу
    //qDebug()<<cport_index.data();
}

void TempGraph::on_btn_connect_port_clicked()
{
   /*if(connect_once){
       QMessageBox::information(this,"Ошибка","Вы уже подключены к порту.\n Сначала отключитесь от старого.");
   }*/
   QString port_name;
   qDebug()<<"Кнопка нажата";
   qDebug()<<cport_index.isValid();
   if(cport_index.isValid() == false)
       QMessageBox::information(this,"Ошибка","Вы не выбрали порт!");
   else
   {
       qDebug()<<"Порт выбран";
        foreach(const QSerialPortInfo& portInfo, QSerialPortInfo::availablePorts())
        {
            qDebug()<<"Вошли в цекл по доступным портам";
            //смотрим, имеет ли порт vendor и device id
            if(portInfo.hasVendorIdentifier() && portInfo.hasProductIdentifier())
            {
                qDebug()<<"Вошли в условие 1";
                //смотрим, совпадают ли соответствуищие id с выбранным в таблице
                if(modelOut->index(cport_index.row(),1).data().toInt() == portInfo.vendorIdentifier() && modelOut->index(cport_index.row(),2).data().toInt() == portInfo.productIdentifier())
                   {
                        qDebug()<< "Вошли в условие 2";
                        //нужна ли данная функция? НЕТ ВОЗМОЖНОСТИ ПРОТЕСТИРОВАТЬ на практике
                        if(portInfo.isBusy())
                        {
                            QMessageBox::information(this,"Ошибка","Данный порт занят!");
                        }
                        qDebug()<<portInfo.portName();
                        port_name = portInfo.portName();
                        break;
                   }
            }
        }
        qDebug() << "вышли из цикла и подключаемся к порту";
        port = new QSerialPort(this);
        port_is_available = true;
        connect_once = true;
             //port->setPort();
            //port->setPortName(port_name);
            qDebug()<<port;
            // qDebug()<<port->portName();
    }

}
