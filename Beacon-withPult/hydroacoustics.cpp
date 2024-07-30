#include "hydroacoustics.h"
#include <QDebug>
//#include "json_parser.h"

Hydroacoustics::Hydroacoustics(QString portName, int baudRate,
                               QObject *parent) : QObject(parent)
{
    ha.setBaudRate(baudRate);
    ha.setPortName(portName);
    ha.setDataBits(QSerialPort::Data8);
    ha.setStopBits(QSerialPort::OneStop);
    ha.setParity(QSerialPort::NoParity);
    ha.setFlowControl(QSerialPort::NoFlowControl);
    ha.open(QIODevice::ReadWrite);


    if (ha.open(QIODevice::ReadWrite)){
        qDebug()<<" port was opened";
    }
    else {
        qDebug()<<" error open port "<< ha.errorString();
    }

    connect(&ha, &QSerialPort::readyRead, this, [this](){

        ha_buffer.append(ha.readAll());
        qDebug() << "ha_buffer " << ha_buffer;
        int size = ha_buffer.size();
        if (size !=0)
        {
                char end = ha_buffer.at(size - 1);
                if (end == 10)
                {
                    uwave.infoModem = ha_buffer;
                    parseBuffer();
                }
        }

    });
}



void Hydroacoustics::readData()
{
//    ha.waitForReadyRead(2000);
//    ha_buffer.append(ha.readAll());
////    qDebug() << ha_buffer;
    //    parseBuffer();
}

void Hydroacoustics::trilat()
{
    float    x1 = 0;
    float    x2 = 0;
    float    x3 = 210;
    //    y1 = 185;
    float    y1 = 1000;
    float    y2 = 0;
    float    y3 = 0;
    //    следующие две формулы будут использоваться, если 1(x1, y1, 0), 2(0, 0, 0), 3(x1, 0, 0)
    uwave.puwv3.distanceIDx = (x3*x3+uwave.puwv3.distanceID2*uwave.puwv3.distanceID2*151300*151300-uwave.puwv3.distanceID3*uwave.puwv3.distanceID3*151300*151300)/(2*x3);
    uwave.puwv3.distanceIDy = (y1*y1 + x1*x1 + (uwave.puwv3.distanceID2*151300)*(uwave.puwv3.distanceID2*151300) - uwave.puwv3.distanceID1*151300*uwave.puwv3.distanceID1*151300)/(2*y1) - uwave.puwv3.distanceIDx*x1/y1;
}

int Hydroacoustics::crc(QByteArray tmp)
{
    int crc_ = 0;
    for (int i = 0; i <= (tmp.size()-1); i++)
    {
        crc_^=tmp[i];
    }
    return crc_;
}

QByteArray Hydroacoustics::crc_MSG(QByteArray tmp)
{
    int crc_ = 0;
    for (int i = 0; i <= tmp.size(); i++)
    {
        crc_^=tmp[i];
    }
    QByteArray crcRes = QByteArray::number(crc_, 16).toUpper();;
    return crcRes;
}

int Hydroacoustics::crc_real(qint8 crc_in)
{
    int crc_real = 0;
    if ((int (ha_buffer[crc_in+1]))>57)
    crc_real += ((int (ha_buffer[crc_in+1]))-'0'-7)*16;
    else crc_real += ((int (ha_buffer[crc_in+1]))-'0')*16;
    if ((int (ha_buffer[crc_in+2]))>57)
    crc_real += ((int (ha_buffer[crc_in+2]))-'0') -7;
    else crc_real += ((int (ha_buffer[crc_in+2]))-'0');
    return crc_real;
}

void Hydroacoustics::sendCmd1()
{
    if (ha.isOpen())
    settings();
    else
    {
        qDebug() << "порт не открыт: " << ha.error();
        ha.close();
        ha.open(QIODevice::ReadWrite);
    }

    if ((uwave.puwv0.errCode == 0) and (uwave.puwv0.cmdID ==6))
    {
        uwave.puwv0.errCode = 0;
        uwave.puwv0.cmdID = 0;
        qDebug()<<"cmd1 sended!";
        emit cmd1Received();
    }
}

void Hydroacoustics::settings()
{
//    Json_parser json;
//    qDebug() << "json.set.mode_package_delivery: " << json.set.mode_package_delivery;
    if (0 == 0)
    {
        char PUWV6[24] = "$PUWV6,1,1,1,1,1,1*32\r\n"; // включаем передачу сообщений о состоянии модема
        qDebug()  <<"bytes written :"<< ha.write(PUWV6, 24);
    }
    else
    {
        char PUWV6[27] = "$PUWV6,1,1000,1,1,1,1*02\r\n"; // включаем передачу сообщений о состоянии модема
        qDebug()  <<"bytes written :"<< ha.write(PUWV6, 27);
    }

    ha.waitForBytesWritten();
}

void Hydroacoustics::sendCmd2()// в этом сообщении настраиваем какой канал слушаем и на какой канал отвечаем
{
    if (ha.isOpen())
    {
//        qDebug() << "snsn";
     //   char PUWV6[18] = "$PUWVF,1,1,0*5E\r\n"; // Настройки пакетного режима 0 адрес
//        char PUWV1[30] = "$PUWV1,0,1,0.,1,1,9.8067*34\r\n";
//        char PUWV1[30] = "$PUWV1,0,0,0.,1,1,9.8067*35\r\n";
        QByteArray array = request_PUWV1(salinity);
        int size = array.size();
        qDebug() << "Проверочка на размер array.size request_PUWV1 " << array.size();
        char *PUWV1 = array.data();
        ha.write(PUWV1, size);
        ha.waitForBytesWritten();
    }
    else
    {
        qDebug() << "порт не открыт: " << ha.error();
        ha.close();
        ha.open(QIODevice::ReadWrite);
    }
    if ((uwave.puwv0.errCode == 0) and (uwave.puwv0.cmdID ==6))
    {
        uwave.puwv0.errCode = 0;
        uwave.puwv0.cmdID = 0;
//        qDebug() << ha_buffer;
//        int size = ha_buffer.size();
//        ha_buffer.remove(0, size);
        qDebug() << ha_buffer;
        qDebug()<<"cmd2 sended!";
        emit initCmd2Done();

    }
}

void Hydroacoustics::modeIdle()
{
//    if (ha.waitForReadyRead(10))
//    {
//        readData();
//    }
//    else
//    {
//        qDebug() << "Данных для чтения нет";
//        emit newMessageDetectedACKIdle(uwave);
    //    }
}

void Hydroacoustics::modeDirect()
{

//    char PUWV2[18] = "$PUWV2,0,0,2*28\r\n"; // запрос
//    char PUWV2[18] = "$PUWV2,1,1,2*28\r\n";
//    char PUWV2[18] = "$PUWV2,1,0,2*29\r\n"; //передача 1 прием 0
//    qDebug()<<"bytes written :" << ha.write(PUWV2, 18);
    QByteArray array = request_PUWV2(chD.txCh, chD.rxCh);
//    qDebug() << "Проверочка на размер array.size request_PUWV2 " << array.size();
    char *PUWV2 = array.data();
    qDebug() << "PUWV2 modeDirect: " << PUWV2;
    qDebug()<<"bytes written :" << ha.write(PUWV2, 18);
    ha.waitForBytesWritten();
}

void Hydroacoustics::modeRound()
{
    qDebug() << "С каким числом модемов будет происходить общение в режиме RoundR: " << chR.Number;

    if ((roundCounter == 1) and (roundCounter <= chR.Number)) {
        qDebug() << "roundCounter == 1 и я общаюсь с: " << chR.txCh1;
//        char PUWV2[18] = "$PUWV2,1,0,2*29\r\n"; //передача 1 прием 0
//        qDebug()<<"bytes written (to beacon 1-0):" << ha.write(PUWV2, 18);
        QByteArray array = request_PUWV2(chR.txCh1, chR.rxCh);
        char *PUWV2 = array.data();
        qDebug() << "PUWV2 modeRound1: " << PUWV2;
        uwave.counterACK1++;

        qDebug()<< "модем "<< chR.txCh1<<"bytes written :" << ha.write(PUWV2, 18);
        qDebug() << "uwave.counterACK1: "<<uwave.counterACK1;
        ha.waitForBytesWritten();
        if (roundCounter < chR.Number) roundCounter ++;
        else roundCounter = 1;
    }
    else if ((roundCounter == 2) and (roundCounter <= chR.Number)) {
//        char PUWV2[18] = "$PUWV2,0,0,2*28\r\n"; // запрос //передача 0 прием 0
//        qDebug()<<"bytes written (to beacon 0-0):" << ha.write(PUWV2, 18);
        qDebug() << "roundCounter == 2 и я общаюсь с: " << chR.txCh2;
        QByteArray array = request_PUWV2(chR.txCh2, chR.rxCh);
        char *PUWV2 = array.data();
        qDebug() << "PUWV2 modeRound2: " << PUWV2;
        uwave.counterACK2++;
        qDebug()<< "модем "<< chR.txCh2 <<"bytes written :" << ha.write(PUWV2, 18);
        qDebug() << "uwave.counterACK2: "<<uwave.counterACK2;
        ha.waitForBytesWritten();
        if (roundCounter < chR.Number) roundCounter ++;
        else roundCounter = 1;
    }
    else if ((roundCounter == 3) and (roundCounter <= chR.Number)) {
//        char PUWV2[18] = "$PUWV2,0,0,2*28\r\n"; // запрос //передача 0 прием 0
//        qDebug()<<"bytes written (to beacon 0-0):" << ha.write(PUWV2, 18);
        qDebug() << "roundCounter == 3 и я общаюсь с: " << chR.txCh3;
        QByteArray array = request_PUWV2(chR.txCh3, chR.rxCh);
        char *PUWV2 = array.data();
        qDebug() << "PUWV2 modeRound3: " << PUWV2;
        uwave.counterACK3++;
        qDebug()<< "модем "<< chR.txCh3 <<"bytes written :" << ha.write(PUWV2, 18);
        qDebug() << "uwave.counterACK3: "<<uwave.counterACK3;
        ha.waitForBytesWritten();
        if (roundCounter < chR.Number) roundCounter ++;
        else roundCounter = 1;
    }
    else if ((roundCounter == 4) and (roundCounter <= chR.Number)) {
//        char PUWV2[18] = "$PUWV2,0,0,2*28\r\n"; // запрос //передача 0 прием 0
//        qDebug()<<"bytes written (to beacon 0-0):" << ha.write(PUWV2, 18);
        qDebug() << "roundCounter == 4 и я общаюсь с: " << chR.txCh4;
        QByteArray array = request_PUWV2(chR.txCh4, chR.rxCh);
        char *PUWV2 = array.data();
        qDebug() << "PUWV2 modeRound4: " << PUWV2;
        uwave.counterACK4++;
        qDebug()<< "модем "<< chR.txCh4 <<"bytes written :" << ha.write(PUWV2, 18);
        qDebug() << "uwave.counterACK2: "<<uwave.counterACK4;
        ha.waitForBytesWritten();
        if (roundCounter < chR.Number) roundCounter ++;
        else roundCounter = 1;
    }
    trilat();
//    clearAll();
//    char PUWV2[18] = "$PUWV2,1,0,2*29\r\n"; //передача 1 прием 0
//    qDebug()<<"bytes written :" << ha.write(PUWV2, 18);
//    ha.waitForBytesWritten();
//    timerRound.start(3000);
//    while ((timerRound.timerId() != 0) or  (uwave.puwv3.rcCmdID = 0) or (uwave.puwv4.rcCmdID = 0)) {}
//    clearAll();
//    timerRound.stop();
//    char PUWV22[18] = "$PUWV2,0,0,2*28\r\n"; // запрос //передача 0 прием 0
//    qDebug()<<"bytes written :" << ha.write(PUWV22, 18);
//    ha.waitForBytesWritten();
//    timerRound.start(3000);
    //    while ((timerRound.timerId() != 0) or  (uwave.puwv3.rcCmdID = 0) or (uwave.puwv4.rcCmdID = 0)) {}
}

void Hydroacoustics::settingsChannelDirect(Channel chDirect)
{
    chD.txCh = chDirect.txCh;
    chD.rxCh = chDirect.rxCh;
}

//void Hydroacoustics::settingsChannelRound(ChannelRound chRoundBC)
//{
//    chR = chRoundBC;
//}


void Hydroacoustics::parseBuffer()
{
    qDebug() << "parseBuffer";
    int count = ha_buffer.count(36);
    qDebug() << count;
    while (count !=0)
    {
        count = count-1;
        qint8 index = ha_buffer.indexOf(36); //поиск индекса $
        if (index == -1)
        {
            // Не найдено сообщение
            qDebug() << "Нет сообщения в буфере";
            return;
        }
        qint8 crc_in = ha_buffer.indexOf(42);
        uint end = crc_in + 5; //последний символ посылки
        QByteArray title = ha_buffer.mid (index+1, 5);
        if (title == "PUWV0")
        {
            QByteArray msg = ha_buffer.mid(index+1, crc_in-1);
            if (crc_real(crc_in) == crc(msg))
            {
                parsePUWV0(msg);
//                updateData(uwave);
                ha_buffer.remove(0, end);
            }
            else
            {
                qDebug () << "PUWV0 crc не верна";
                ha_buffer.remove(0, end);
            }
        }
        if (title == "PUWVE")
        {
            QByteArray msg = ha_buffer.mid(index+1, crc_in-1);
            if (crc_real(crc_in) == crc(msg))
            {
                parsePUWVE(msg);
//                updateData(uwave);
                ha_buffer.remove(0, end);
            }
            else
            {
                qDebug () << "PUWVE crc не верна";
                ha_buffer.remove(0, end);
            }
        }
        if (title == "PUWV7")
        {
            QByteArray msg = ha_buffer.mid(index+1, crc_in-1);
            if (crc_real(crc_in) == crc(msg))
            {
                parsePUWV7(msg);
//                updateData(uwave);
                ha_buffer.remove(0, end);
            }
            else
            {
                qDebug () << "PUWV7 crc не верна";
                ha_buffer.remove(0, end);
            }
        }
        if (title == "PUWV4")
        {
            QByteArray msg = ha_buffer.mid(index+1, crc_in-1);
            if (crc_real(crc_in) == crc(msg))
            {
                parsePUWV4(msg);
//                updateData(uwave);
                ha_buffer.remove(0, end);
            }
            else
            {
                qDebug () << "PUWV4 crc не верна";
                ha_buffer.remove(0, end);
            }
        }
        if (title == "PUWV3")
        {
            QByteArray msg = ha_buffer.mid(index+1, crc_in-1);
            if (crc_real(crc_in) == crc(msg))
            {
                parsePUWV3(msg);
//                updateData(uwave);
                ha_buffer.remove(0, end);
            }
            else
            {
                qDebug () << "PUWV3 crc не верна";
                ha_buffer.remove(0, end);
            }
        }

    }
    int size = ha_buffer.size();
    ha_buffer.remove(0, size);
    emit updateData(uwave);
//    clearAll();

}

void Hydroacoustics::parsePUWV0(QByteArray msg)
{
    int index =msg.indexOf(44);//ищем первую запятую, перед ней идет не интересный заголовок
    msg.remove(0, index+1); // удаляем заголовок
    index =msg.indexOf(44);//ищем запятую
    uwave.puwv0.cmdID = atoi(msg.mid(0, 1));
    qDebug() << "puwv0.cmdID: "<< uwave.puwv0.cmdID;
    msg.remove(0, index+1);
    index =msg.indexOf(42);//ищем звездочку *
    uwave.puwv0.errCode = atoi(msg.mid(0, 1));
    qDebug() << "puwv0.errCode: "<< uwave.puwv0.errCode;
    msg.remove(0, index+1);
    if ((uwave.puwv0.errCode == 0) and (uwave.puwv0.cmdID ==2))
    {
        uwave.counterACK++;
        qDebug() << "uwave.counterACK++ отправлено сообещние: " << uwave.counterACK;
    }

}

void Hydroacoustics::parsePUWVE(QByteArray msg)
{
    qDebug() << msg;
    int index =msg.indexOf(44);//ищем первую запятую, перед ней идет не интересный заголовок
    msg.remove(0, index+1); // удаляем заголовок
    index =msg.indexOf(44);//ищем запятую
    uwave.puwve.isPTMode = atoi(msg.mid(0, 1));
    qDebug() << "puwve.isPTMode: "<< uwave.puwve.isPTMode;
    msg.remove(0, index+1);
    index =msg.indexOf(42);//ищем звездочку *
    uwave.puwve.ptLocalAddress = atoi(msg.mid(0, 1));
    qDebug() << "puwve.ptLocalAddress: "<< uwave.puwve.ptLocalAddress;
    msg.remove(0, index+1);
}

void Hydroacoustics::parsePUWV7(QByteArray msg)
{
    qDebug() << msg;
    int index =msg.indexOf(44);//ищем первую запятую, перед ней идет не интересный заголовок
    msg.remove(0, index+1); // удаляем заголовок
    index =msg.indexOf(44);//ищем запятую
    uwave.puwv7.Pressure_mBar = atof(msg.mid(0, index));
    qDebug() << "puwv7.Pressure_mBar: " << uwave.puwv7.Pressure_mBar;
    msg.remove(0, index+1);
//    qDebug() << msg;
    index =msg.indexOf(44);//ищем запятую
    uwave.puwv7.Temperature_C = atof(msg.mid(0, index));
    uwave.puwv3.temp = uwave.puwv7.Temperature_C;
    qDebug() << "puwv7.Temperature_C: " << uwave.puwv7.Temperature_C;
    msg.remove(0, index+1);
//    qDebug() << msg;
    index =msg.indexOf(44);//ищем запятую
    uwave.puwv7.Depth_m = atof(msg.mid(0, index));
    qDebug() << "puwv7.Depth_m: " << uwave.puwv7.Depth_m;
    msg.remove(0, index+1);
//    qDebug() << msg;
    index =msg.indexOf(44);//ищем запятую
    uwave.puwv7.VCC_V = atof(msg.mid(0, index));
    qDebug() << "puwv7.VCC_V: " << uwave.puwv7.VCC_V;
    msg.remove(0, index+1);
}

void Hydroacoustics::parsePUWV4(QByteArray msg)
{
    qDebug() << msg;
    int index =msg.indexOf(44);//ищем первую запятую, перед ней идет не интересный заголовок
    msg.remove(0, index+1); // удаляем заголовок
    index =msg.indexOf(44);//ищем запятую
    uwave.puwv4.txChID = atoi(msg.mid(0, 1));
    qDebug() << "puwv4.txChID: "<< uwave.puwv4.txChID;
    msg.remove(0, index+1);
    index =msg.indexOf(42);//ищем звездочку *
    uwave.puwv4.rcCmdID = atoi(msg.mid(0, 1));
    qDebug() << "puwv4.rcCmdID: "<< uwave.puwv4.rcCmdID;
    msg.remove(0, index+1);
    uwave.puwv4.counterAll++;
    if (uwave.puwv4.txChID == 1){
        uwave.puwv4.counterID1++;
    }
    else if (uwave.puwv4.txChID == 2){
        uwave.puwv4.counterID2++;
    }
    else if (uwave.puwv4.txChID == 3){
            uwave.puwv4.counterID3++;
        }
    else if (uwave.puwv4.txChID == 4){
        uwave.puwv4.counterID4++;
    }
    else {
        uwave.puwv4.counter++;
    }
}

void Hydroacoustics::parsePUWV3(QByteArray msg)
{
    qDebug() << msg;
    int index =msg.indexOf(44);//ищем первую запятую, перед ней идет не интересный заголовок
    msg.remove(0, index+1); // удаляем заголовок
    index =msg.indexOf(44);//ищем запятую
    uwave.puwv3.txChID = atoi(msg.mid(0, 1));
    qDebug() << "puwv3.txChID: "<< uwave.puwv3.txChID;
    msg.remove(0, index+1);
    index =msg.indexOf(44);//ищем запятую
    uwave.puwv3.rcCmdID = atoi(msg.mid(0, 1));
    qDebug() << "puwv3.rcCmdID: "<< uwave.puwv3.rcCmdID;
    msg.remove(0, index+1);
    index =msg.indexOf(44);//ищем запятую
    uwave.puwv3.propTime = atof(msg.mid(0, index));
    qDebug() << "puwv3.propTime: "<< uwave.puwv3.propTime;
    msg.remove(0, index+1);
    index =msg.indexOf(44);//ищем запятую
    uwave.puwv3.MSR = atof(msg.mid(0, index));
    qDebug() << "puwv3.MSR: "<< uwave.puwv3.MSR;
    msg.remove(0, index+1);
    index =msg.indexOf(44);//ищем запятую
    uwave.puwv3.Value = atof(msg.mid(0, index));
    qDebug() << "puwv3.Value: "<< uwave.puwv3.Value;
    msg.remove(0, index+1);
    uwave.puwv3.counterAll++;
    if(uwave.puwv3.txChID ==1){
        uwave.puwv3.distanceID1 = uwave.puwv3.propTime;
        uwave.puwv3.counterID1++;
        uwave.puwv3.tempID1 = uwave.puwv3.Value;
        qDebug() << "uwave.puwv3.tempID1: " << uwave.puwv3.tempID1;
    }

    else if(uwave.puwv3.txChID ==2){
        uwave.puwv3.distanceID2 = uwave.puwv3.propTime;
        uwave.puwv3.counterID2++;
        uwave.puwv3.tempID2 = uwave.puwv3.Value;
        qDebug() << "uwave.puwv3.tempID2: " << uwave.puwv3.tempID2;
    }

    else if(uwave.puwv3.txChID ==3){
        uwave.puwv3.distanceID3 = uwave.puwv3.propTime;
        uwave.puwv3.counterID3++;
        uwave.puwv3.tempID3 = uwave.puwv3.Value;
        qDebug() << "uwave.puwv3.tempID3: " << uwave.puwv3.tempID3;
    }

    else if(uwave.puwv3.txChID ==4){
        uwave.puwv3.distanceID4 = uwave.puwv3.propTime;
        uwave.puwv3.counterID4++;
        uwave.puwv3.tempID4 = uwave.puwv3.Value;
        qDebug() << "uwave.puwv3.tempID4: " << uwave.puwv3.tempID4;
    }
    else {
        uwave.puwv3.distance = uwave.puwv3.propTime;
        uwave.puwv3.counter++;
    }

}

void Hydroacoustics::stopCounter()
{
    qDebug() << "stopCounter";
    uwave.puwv4.counter = 0;
    uwave.puwv3.counter = 0;
    uwave.counterACK = 0;
    uwave.puwv3.counterID1 = 0;
    uwave.puwv3.counterID2 = 0;
    uwave.puwv3.counterID3 = 0;
    uwave.puwv3.counterID4 = 0;
    uwave.puwv3.counterAll = 0;
    uwave.puwv4.counterID1 = 0;
    uwave.puwv4.counterID2 = 0;
    uwave.puwv4.counterID3 = 0;
    uwave.puwv4.counterID4 = 0;
    uwave.puwv4.counterAll = 0;
    uwave.counterACK1 = 0;
    uwave.counterACK2 = 0;
    uwave.counterACK3 = 0;
    uwave.counterACK4 = 0;

}

void Hydroacoustics::clearAll()
{
    uwave.puwve.isPTMode = 0;
    uwave.puwve.ptLocalAddress = 0;
    uwave.puwv0.cmdID = 0;
    uwave.puwv0.errCode = 0;
    uwave.puwv4.txChID = 0;
    uwave.puwv4.rcCmdID = 0;
}

QByteArray Hydroacoustics::request_PUWV2(int idModem,int idChennel)
{
    //$PUWV2,1,0,2*29\r\n
    QByteArray command = "$PUWV2,";
    command.append(QByteArray::number(idModem));
    command.append(",");
    command.append(QByteArray::number(idChennel));
    command.append(",3*");
    qint8 end = command.size();
    QByteArray msg = command.mid(1, end-2);
    qDebug() << "msg: " << msg;
    command.append(crc_MSG(msg));
    command.append("\r\n");
    qDebug() << "command PUWV2: " << command;
    return command;
}

QByteArray Hydroacoustics::request_PUWV1(float STY, int idModem, int idChennel, float gravityAcc)
{
//    char PUWV1[30] = "$PUWV1,0,0,0.,1,1,9.8067*35\r\n";
    QByteArray command = "$PUWV1,";
    command.append(QByteArray::number(idModem));
    command.append(",");
    command.append(QByteArray::number(idChennel));
    command.append(",");
    command.append(QByteArray::number(STY));
    command.append(",1,1,");
    command.append(QByteArray::number(gravityAcc));
    command.append("*");
    qint8 end = command.size();
    QByteArray msg = command.mid(1, end-2);
    qDebug() << "msg: " << msg;
    command.append(crc_MSG(msg));
    command.append("\r\n");
    qDebug() << "command PUWV1: " << command;
    return command;
};
