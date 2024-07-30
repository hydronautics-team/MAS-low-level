#include "nmea0183.h"

NMEA0183::NMEA0183(QString portName, int baudRate, QObject *parent)
{
    gps_port.setBaudRate(baudRate);
    gps_port.setPortName(portName);
    gps_port.open(QIODevice::ReadOnly);
    connect (&gps_port, &QSerialPort::readyRead, this, &NMEA0183::readData);
//    connect (&timer, &QTimer::timeout, this, &NMEA0183::readData);
//    timer.start(30);

}

void NMEA0183::readData()
{
    gps_buffer.append(gps_port.readAll());
    qDebug() << "gps_buffer: " << gps_buffer;
    parseBuffer();
    emit newMessageDetected(gps);
}

void NMEA0183::parseBuffer()
{
    if (gps_buffer.size () < 6)
        return;//определяем что буфер не пустой
    int index =gps_buffer.indexOf(36); //поиск индекса $
//    qDebug () << "index$:   " << index;
    if (index == -1)
    {
        // Не найдено сообщение
        qDebug() << "Нет сообщения в буфере";
        return;
    }
    int count = gps_buffer.count(36);
    qDebug() << count;
    while (count !=0)
    {
        count = count-1;
        int index =gps_buffer.indexOf(36); //поиск индекса $
        uint crc_in =gps_buffer.indexOf(42); //поиск * после которой идет контрольная сумма
        uint end = crc_in + 5; //последний символ посылки
    //    qDebug() << "endSimvol:    " << end;
        QByteArray tmp = gps_buffer.mid(index+1, 5);
        qDebug() << tmp;
        if ((tmp == GPRMC) or (tmp == "GNRMC"))
        {
            QByteArray msg = gps_buffer.mid(index+1, crc_in-1);
            int crc_real = 0;
            if ((int (gps_buffer[crc_in+1]))>57)
            crc_real += ((int (gps_buffer[crc_in+1]))-'0'-7)*16;
            else crc_real += ((int (gps_buffer[crc_in+1]))-'0')*16;
            if ((int (gps_buffer[crc_in+2]))>57)
            crc_real += ((int (gps_buffer[crc_in+2]))-'0') -7;
            else crc_real += ((int (gps_buffer[crc_in+2]))-'0');
    //        qDebug() << "crc_realGPRMC: " << crc_real;
            if (crc_real == crc(msg))//проверка чек-суммы
            {
                parseGPRMC(msg);
                qDebug() << msg;
                gps_buffer.remove(0, end);
                qDebug() << "GPRMC";
            }
            else
            {
                qDebug () << "GPRMC crc не верна";
                gps_buffer.remove(0, end);
            }
        }
        if ((tmp == GPVTG) or (tmp == "GNVTG"))
        {
            QByteArray msg = gps_buffer.mid(index+1, crc_in-1);
            int crc_real = 0;
            if ((int (gps_buffer[crc_in+1]))>57)
            crc_real += ((int (gps_buffer[crc_in+1]))-'0'-7)*16;
            else crc_real += ((int (gps_buffer[crc_in+1]))-'0')*16;
            if ((int (gps_buffer[crc_in+2]))>57)
            crc_real += ((int (gps_buffer[crc_in+2]))-'0') -7;
            else crc_real += ((int (gps_buffer[crc_in+2]))-'0');
    //        qDebug() << "crc_realGPVTG: " << crc_real;
            if (crc_real == crc(msg))//проверка чек-суммы
            {
                parseGPVTG(msg);
                gps_buffer.remove(0, end);
                qDebug() << "GPVTG";
            }
            else
            {
                qDebug () << "GPVTG crc не верна";
                gps_buffer.remove(0, end);
            }
        }
        if ((tmp == GPTXT) or (tmp == "GNTXT"))
        {
            QByteArray msg = gps_buffer.mid(index+1, crc_in-1);
            int crc_real = 0;
            if ((int (gps_buffer[crc_in+1]))>57)
            crc_real += ((int (gps_buffer[crc_in+1]))-'0'-7)*16;
            else crc_real += ((int (gps_buffer[crc_in+1]))-'0')*16;
            if ((int (gps_buffer[crc_in+2]))>57)
            crc_real += ((int (gps_buffer[crc_in+2]))-'0') -7;
            else crc_real += ((int (gps_buffer[crc_in+2]))-'0');
    //        qDebug() << "crc_realGPTXT: " << crc_real;
            if (crc_real == crc(msg))//проверка чек-суммы
            {
                parseGPTXT(msg);
                qDebug() << "GPTXT:" << msg;
                gps_buffer.remove(0, end);
           //     qDebug() << "GPTXT:" << msg;
            }
            else
            {
                qDebug () << "GPTXT crc не верна";
                gps_buffer.remove(0, end);
            }
        }
        if ((tmp == GPGGA) or (tmp == "GNGGA"))
        {
            QByteArray msg = gps_buffer.mid(index+1, crc_in-1);
            int crc_real = 0;
    //        qDebug() << int (gps_buffer[crc_in+1]);
    //        qDebug() << int (gps_buffer[crc_in+2]);
            if ((int (gps_buffer[crc_in+1]))>57)
            crc_real += ((int (gps_buffer[crc_in+1]))-'0'-7)*16;
            else crc_real += ((int (gps_buffer[crc_in+1]))-'0')*16;
            if ((int (gps_buffer[crc_in+2]))>57)
            crc_real += ((int (gps_buffer[crc_in+2]))-'0') -7;
            else crc_real += ((int (gps_buffer[crc_in+2]))-'0');
    //        qDebug() << "crc_realGPGGA: " << crc_real;
            if (crc_real == crc(msg))//проверка чек-суммы
            {
                parseGPGGA(msg);
                gps_buffer.remove(0, end);
                qDebug() << "GPGGA";
            }
            else
            {
                qDebug () << "GPGGA crc не верна";
                gps_buffer.remove(0, end);
            }
        }
        if ((tmp == GPGSA) or (tmp == "GNGSA"))
        {
            QByteArray msg = gps_buffer.mid(index+1, crc_in-1);
            int crc_real = 0;
            if ((int (gps_buffer[crc_in+1]))>57)
            crc_real += ((int (gps_buffer[crc_in+1]))-'0'-7)*16;
            else crc_real += ((int (gps_buffer[crc_in+1]))-'0')*16;
            if ((int (gps_buffer[crc_in+2]))>57)
            crc_real += ((int (gps_buffer[crc_in+2]))-'0') -7;
            else crc_real += ((int (gps_buffer[crc_in+2]))-'0');
    //        qDebug() << "crc_realGPGSA: " << crc_real;
            if (crc_real == crc(msg))//проверка чек-суммы
            {
                parseGPGSA(msg);
                gps_buffer.remove(0, end);
                qDebug() << "GPGSA";
            }
            else
            {
                qDebug () << "GPGSA crc не верна";
                gps_buffer.remove(0, end);
            }
        }
        if ((tmp == GPGSV) or (tmp == "GNGSV"))
        {
            QByteArray msg = gps_buffer.mid(index+1, crc_in-1);
            int crc_real = 0;
            if ((int (gps_buffer[crc_in+1]))>57)
            crc_real += ((int (gps_buffer[crc_in+1]))-'0'-7)*16;
            else crc_real += ((int (gps_buffer[crc_in+1]))-'0')*16;
            if ((int (gps_buffer[crc_in+2]))>57)
            crc_real += ((int (gps_buffer[crc_in+2]))-'0') -7;
            else crc_real += ((int (gps_buffer[crc_in+2]))-'0');
    //        qDebug() << "crc_realGPGSV: " << crc_real;
            if (crc_real == crc(msg))//проверка чек-суммы
            {
                parseGPGSV(msg);
                gps_buffer.remove(0, end);
                qDebug() << "GPGSV";
            }
            else
            {
                qDebug () << "GPGSV crc не верна";
                gps_buffer.remove(0, end);
            }
        }
        if ((tmp == GPGLL) or (tmp == "GNGLL"))
        {
            QByteArray msg = gps_buffer.mid(index+1, crc_in-1);
            int crc_real = 0;
            if ((int (gps_buffer[crc_in+1]))>57)
            crc_real += ((int (gps_buffer[crc_in+1]))-'0'-7)*16;
            else crc_real += ((int (gps_buffer[crc_in+1]))-'0')*16;
            if ((int (gps_buffer[crc_in+2]))>57)
            crc_real += ((int (gps_buffer[crc_in+2]))-'0') -7;
            else crc_real += ((int (gps_buffer[crc_in+2]))-'0');
    //        qDebug() << "crc_realGPGLL: " << crc_real;
            if (crc_real == crc(msg))//проверка чек-суммы
            {
                parseGPGLL(msg);
                gps_buffer.remove(0, end);
                qDebug() << "GPGLL";
            }
            else
            {
                qDebug () << "GPGLL crc не верна";
                gps_buffer.remove(0, end);
            }
        }

        gps_buffer.remove(0, end);
        qDebug() << "gps_buffer.size:   " << gps_buffer.size();
    }
}

int NMEA0183::crc(QByteArray tmp)
{
    int crc_ = 0;
    for (int i = 0; i <= tmp.size(); i++)
        crc_^=tmp[i];
//    qDebug() << "crc: " << crc_;
    return crc_;
}

void NMEA0183::parseGPRMC(QByteArray msg)
{
    int index =msg.indexOf(44);//ищем первую запятую
    msg.remove(0, index+1);
    index =msg.indexOf(44);//ищем запятую
    if (index == 0)
    {
        msg.remove(0, index+1);
    }
    if (index > 0)
    {
        gps.rms.time = QTime::fromString((msg.mid(0, index-1)), "hhmmss.z");
        msg.remove(0, index+1);
    }
    index =msg.indexOf(44);//ищем запятую
    if (index == 0)
    {
        gps.rms.status = 0;
        msg.remove(0, index+1);
    }
    if (index > 0)
    {
        gps.rms.status = (char)msg[0];
        msg.remove(0, index+1);
    }
    index =msg.indexOf(44);//ищем запятую
    if (index == 0)
    {
        gps.rms.lat = 0;
        msg.remove(0, index+1);
    }
    if (index > 0)
    {
        gps.rms.lat = atof(msg.mid(0, index-1));
        msg.remove(0, index+1);
    }
    index =msg.indexOf(44);//ищем запятую
    if (index == 0)
    {
        gps.rms.NS = 0;
        msg.remove(0, index+1);
    }
    if (index > 0)
    {
        gps.rms.NS = (char)msg[0];
        msg.remove(0, index+1);
    }
    index =msg.indexOf(44);//ищем запятую
    if (index == 0)
    {
        gps.rms.long_ = 0;
        msg.remove(0, index+1);
    }
    if (index > 0)
    {
        gps.rms.long_ = atof(msg.mid(0, index-1));
        msg.remove(0, index+1);
    }
    index =msg.indexOf(44);//ищем запятую
    if (index == 0)
    {
        gps.rms.EW = 0;
        msg.remove(0, index+1);
    }
    if (index > 0)
    {
        gps.rms.EW = (char)msg[0];
        msg.remove(0, index+1);
    }
    index =msg.indexOf(44);//ищем запятую
    if (index == 0)
    {
        gps.rms.spd = 0;
        msg.remove(0, index+1);
    }
    if (index > 0)
    {
        gps.rms.spd = atof(msg.mid(0, index-1));
        msg.remove(0, index+1);
    }
    index =msg.indexOf(44);//ищем запятую
    if (index == 0)
    {
        gps.rms.cog = 0;
        msg.remove(0, index+1);
    }
    if (index > 0)
    {
        gps.rms.cog = atof(msg.mid(0, index-1));
        msg.remove(0, index+1);
    }
    index =msg.indexOf(44);//ищем запятую
//    qDebug() << msg;
    if (index == 0)
    {
        msg.remove(0, index+1);
    }
    if (index > 0)
    {
//        qDebug() << msg.mid(0, index);
        gps.rms.date = QDate::fromString((msg.mid(0, index)), "ddMMyy");
        msg.remove(0, index+1);
//        qDebug() << gps.rms.date.toString();
    }
    index =msg.indexOf(44);//ищем запятую
    if (index == 0)
    {
        gps.rms.mv = 0;
        msg.remove(0, index+1);
    }
    if (index > 0)
    {
        gps.rms.mv = atof(msg.mid(0, index-1));
        msg.remove(0, index+1);
    }
    index =msg.indexOf(44);//ищем запятую
    if (index == 0)
    {
        gps.rms.mvEW = 0;
        msg.remove(0, index+1);
    }
    if (index > 0)
    {
        gps.rms.mvEW = (char)msg[0];
        msg.remove(0, index+1);
    }
    index =msg.indexOf(44);//ищем запятую
    if (index == 0)
    {
        gps.rms.posMode = 0;
        msg.remove(0, index+1);
    }
    if (index > 0)
    {
        gps.rms.posMode = (char)msg[0];
        msg.remove(0, index+1);
    }
    gps.rms.counter +=1;
    qDebug() << "gps.rms.time:    " << gps.rms.time.toString("hhmmss.z");
    qDebug() << "gps.rms.status:  " << gps.rms.status;
    qDebug() << "gps.rms.lat:     " << gps.rms.lat;
    qDebug() << "gps.rms.NS:      " << gps.rms.NS;
    qDebug() << "gps.rms.long:    " << gps.rms.long_;
    qDebug() << "gps.rms.EW:      " << gps.rms.EW;
    qDebug() << "gps.rms.spd:     " << gps.rms.spd;
    qDebug() << "gps.rms.cog:     " << gps.rms.cog;
    qDebug() << "gps.rms.date:    " << gps.rms.date.toString("dd.MM.yy");
    qDebug() << "gps.rms.mv:      " << gps.rms.mv;
    qDebug() << "gps.rms.mvEW:    " << gps.rms.mvEW;
    qDebug() << "gps.rms.posMode: " << gps.rms.posMode;
    qDebug() << "gps.rms.counter: " << gps.rms.counter;

}

void NMEA0183::parseGPVTG(QByteArray msg)
{

}

void NMEA0183::parseGPTXT(QByteArray msg)
{

}

void NMEA0183::parseGPGGA(QByteArray msg)
{

}

void NMEA0183::parseGPGSA(QByteArray msg)
{

}

void NMEA0183::parseGPGSV(QByteArray msg)
{

}

void NMEA0183::parseGPGLL(QByteArray msg)
{
    int index =msg.indexOf(44);//ищем первую запятую
//    qDebug() << "msg: " << msg;
//    qDebug() << "index первая запятая: " << index;
    msg.remove(0, index+1);
    index =msg.indexOf(44);//ищем запятую
//    qDebug() << "index 2 запятая: " << index;
//    qDebug() << "msg: " << msg;
        if (index == 0)
        {
            gps.gll.lat = 0;
            msg.remove(0, index+1);
        }
        if (index > 0)
        {
            gps.gll.lat = atof(msg.mid(0, index-1));
            msg.remove(0, index+1);
        }
//        qDebug() << "gll.lat: " << gll.lat;

    index =msg.indexOf(44);//ищем запятую
//    qDebug() << "index 3 запятая: " << index;
//    qDebug() << "msg: " << msg;
        if (index == 0)
        {
            gps.gll.NS = 0;
            msg.remove(0, index+1);
        }
        if (index > 0)
        {
            gps.gll.NS = (char)msg[0];
            msg.remove(0, index+1);
        }
//        qDebug() << "gll.NS: " << gll.NS;

        index =msg.indexOf(44);//ищем запятую
//        qDebug() << "index 4 запятая: " << index;
//        qDebug() << "msg: " << msg;
            if (index == 0)
            {
                gps.gll.long_ = 0;
                msg.remove(0, index+1);
            }
            if (index > 0)
            {
                gps.gll.long_ = atof(msg.mid(0, index-1));
                msg.remove(0, index+1);
            }
//            qDebug() << "gll.long_: " << gll.long_;

        index =msg.indexOf(44);//ищем запятую
//        qDebug() << "index 5 запятая: " << index;
//        qDebug() << "msg: " << msg;
            if (index == 0)
            {
                gps.gll.EW = 0;
                msg.remove(0, index+1);
            }
            if (index > 0)
            {
                gps.gll.EW = (char)msg[0];
                msg.remove(0, index+1);
            }
//            qDebug() << "gll.EW: " << gll.EW;

        index =msg.indexOf(44);//ищем запятую
//        qDebug() << "index 6 запятая: " << index;
//        qDebug() << "msg: " << msg;
            if (index == 0)
            {
          //      gps.gll.time = 0;
                msg.remove(0, index+1);
            }
            if (index > 0)
            {
               // gps.gll.time = atof(msg.mid(0, index-1));
//                gps.gll.time_h = atoi(msg.mid(0, 2));
//                gps.gll.time_m = atoi(msg.mid(2, 2));
//                gps.gll.time_s = atoi(msg.mid(4, 2));
//                gps.gll.time_ms = atoi(msg.mid(7, 2));
                gps.gll.time = QTime::fromString((msg.mid(0, index-1)), "hhmmss.z");
                msg.remove(0, index+1);
            }
//            qDebug() << "gll.time: " << gll.time;

        index =msg.indexOf(44);//ищем запятую
//        qDebug() << "index 7 запятая: " << index;
//        qDebug() << "msg: " << msg;
            if (index == 0)
            {
                gps.gll.status = 0;
                msg.remove(0, index+1);
            }
            if (index > 0)
            {
                gps.gll.status = (char)msg[0];
                msg.remove(0, index+1);
            }
//            qDebug() << "gll.status: " << gll.status;
//        qDebug() << "index 8 запятая: " << index;
//        qDebug() << "msg: " << msg;
            if (index == 0)
            {
                gps.gll.posMode = 0;
                msg.remove(0, index+1);
            }
            if (index > 0)
            {
                gps.gll.posMode = (char)msg[0];
                msg.remove(0, index+1);
            }
//            qDebug() << "gll.posMode: " << gll.posMode;
        qDebug() << "gps.gll.lat:       " << gps.gll.lat;
        qDebug() << "gps.gll.NS:        " << gps.gll.NS;
        qDebug() << "gps.gll.long_:     " << gps.gll.long_;
        qDebug() << "gps.gll.EW:        " << gps.gll.EW;
//        qDebug() << "gps.gll.time:      " << gps.gll.time_h << ":" << gps.gll.time_m << ":" \
//                 << gps.gll.time_s << ":" << gps.gll.time_ms;
        qDebug() << "gps.gll.time:      " << gps.gll.time;
        qDebug() << "gps.gll.status:    " << gps.gll.status;
        qDebug() << "gps.gll.posMode:   " << gps.gll.posMode;
}
