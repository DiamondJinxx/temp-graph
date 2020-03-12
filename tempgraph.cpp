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
    tmr_serialread    = new QTimer();
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
    //то , что таймер запроса данных и таймер отрисовка графика совпадают интервалами срабатывания внесет ошибку
    tmr_serialread->setInterval(1000);
    ui->lbl_port_status->hide();//скрываем индикацию, пока нет подключения к порту
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
    connect(tmr_serialread,SIGNAL(timeout()),this,SLOT(serialRead()));

    tmrupgraph->start();
    tmr_up_ports->start();
    tmr_serialread->start();
}

TempGraph::~TempGraph()
{
    if(port->isOpen()){
        port->close();
        qDebug()<<"Порт закрыт вместе с закрытием программы";
    }
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
       info.append("Имя порта:\t"+portInfo.portName()+"\n");
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
    qDebug() << (cur_port_index.isValid());
    cur_port_index = ui->tblOut->currentIndex();
    qDebug()<<cur_port_index.isValid();
    //попытки выцепить данные напрямую через получаемый индекс
    //qDebug()<<cport_index;
    qDebug()<<cur_port_index.row();
    //qDebug()<<modelOut->index(cport_index.row(),1).data().toInt(); // позволяет выцепить данные по индексу модели и сразу преобразовать к целому типу
    //qDebug()<<cport_index.data();
}

void TempGraph::on_btn_connect_port_clicked()
{
   if(connect_once && con_port_index.row() == ui->tblOut->currentIndex().row())
   {
      QMessageBox::information(this,"Ошибка","Вы уже подключены к этому порту.");
      return;
   }
   if(connect_once){
       QMessageBox::information(this,"Ошибка","Вы уже подключены к порту.\n Сначала отключитесь от старого.");
       return;
   }
   QString port_name;
   qDebug()<<"Кнопка нажата";
   qDebug()<<cur_port_index.isValid();
   if(cur_port_index.isValid() == false)
       QMessageBox::information(this,"Ошибка","Вы не выбрали порт!");
   else
   {
       qDebug()<<"Порт выбран";
       QSerialPortInfo port_to_connect;
        foreach(const QSerialPortInfo& portInfo, QSerialPortInfo::availablePorts())
        {
            qDebug()<<"Вошли в цекл по доступным портам";
            //смотрим, имеет ли порт vendor и device id
            if(portInfo.hasVendorIdentifier() && portInfo.hasProductIdentifier())
            {
                qDebug()<<"Вошли в условие 1";
                //смотрим, совпадают ли соответствуищие id с выбранным в таблице
                if(modelOut->index(cur_port_index.row(),1).data().toInt() == portInfo.vendorIdentifier() && modelOut->index(cur_port_index.row(),2).data().toInt() == portInfo.productIdentifier())
                   {
                        qDebug()<< "Вошли в условие 2";
                        //нужна ли данная функция? НЕТ ВОЗМОЖНОСТИ ПРОТЕСТИРОВАТЬ на практике
                        if(portInfo.isBusy())
                        {
                            QMessageBox::information(this,"Ошибка","Данный порт занят!");
                            qDebug()<< "Порт занят";
                            return;
                        }
                        qDebug()<<"Нашли нужный порт";
                        qDebug()<<portInfo.portName();
                        port_to_connect = portInfo;
                        //port_name = portInfo.portName();
                        port_is_available = true;
                        qDebug()<<"Порт доступен для соединениия?" << (port_is_available ? "да":"нет");
                        break;
                   }
            }
        }
        qDebug() << "вышли из цикла,выделяем память под объект порта";
        qDebug()<<"Порт для соединения: "<<port_to_connect.portName();
        port = new QSerialPort(this);
        connect_once = true;
        qDebug()<<"Зафикисировали соединения с портом вообще?"<< (connect_once ? "да":"нет");
        con_port_index = cur_port_index;//для пресечения повторного подключения

        /* блок настройки порта */
        if(port_is_available){
            port->setPort(port_to_connect);

            if(port_to_connect.description().length() == 0)
                ui->lbl_connect->setText("Соединение с портом: \n  Системный \t" + port_to_connect.portName());
            else
                ui->lbl_connect->setText("Соединение с портом: \n" + port_to_connect.description() + "\t" + port_to_connect.portName());
            ui->lbl_port_status->setText("Порт закрыт");
            ui->lbl_port_status->show();
            qDebug()<<"Указали порт к подключению.";
            qDebug()<<"Открываем порт";
            if(port->open(QIODevice::ReadOnly)){
                qDebug()<<"Порт открыт только для чтения";
                ui->lbl_port_status->setText("Порт открыт");
                ui->lbl_port_status->show();
            }
            port->setBaudRate(QSerialPort::Baud9600);//установка скорости обменна данными
            port->setDataBits(QSerialPort::Data8);//установка бита данных
            port->setFlowControl(QSerialPort::NoFlowControl);//установка контроля потока
            port->setParity(QSerialPort::NoParity);  //установка бита четности
            port->setStopBits(QSerialPort::OneStop); //
        }
        else{
            qDebug()<<"Ошибка доступа.Нужный порт не найден/не доступен";
            QMessageBox::information(this,"Ошибка подключения к порту","Невозможно открыть данный порт");
        }

    }
}

void TempGraph::on_btn_desconnect_port_clicked()
{
    if(connect_once == false)
        QMessageBox::information(this,"Ошибка","Вы не подключены ни к одному порту");
    else
    {
        if(port->isOpen()){
            qDebug()<<"Порт "+port->portName()+" закрывается";
            port->close();
            QString c = (port->isOpen() ? "открыт":"закрыт");

            qDebug()<<"Состояние порта:  " + c;
            connect_once = false;
            port_is_available = false;
            ui->lbl_connect->setText("Соединеия нет");
            ui->lbl_port_status->setText("Порт закрыт");
            ui->lbl_port_status->hide();
        }
    }
}

void TempGraph::serialRead()
{
    //qDebug()<<req<<connect_once;
    if(req && connect_once)
    {
        qDebug()<<"Чтение с порта каждую секунду";
    }
}
void TempGraph::on_btn_query_clicked()
{
   req = true;
   qDebug()<<req<<connect_once;
   qDebug()<<"Наличие запроса данных от МК"<<req;
}

void TempGraph::on_btn_stopquery_clicked()
{
   req = false;
   qDebug()<<req<<connect_once;
   qDebug()<<"Отсутствие запроса данных от МК"<<req;
}

