#include "logger.h"
#include <QTime>
#include <QDebug>

Logger::Logger(QObject *parent) : QObject{parent}
{

}

void Logger::logTick(DataFromAH127C data)
{
    if(writeLog)
    {
        QTextStream stream (&file);
        stream << data.yaw << ", " <<
                  data.pitch << ", " <<
                  data.roll << ", " <<
                  data.X_accel << ", " <<
                  data.Y_accel << ", " <<
                  data.Z_accel << ", " <<
                  data.X_rate << ", " <<
                  data.Y_rate << ", " <<
                  data.Z_rate << ", " <<
                  data.X_magn << ", " <<
                  data.Y_magn << ", " <<
                  data.Z_magn << ", " <<
                  data.first_qvat << ", " <<
                  data.second_qvat << ", " <<
                  data.third_qvat << ", " <<
                  data.four_qvat
               << "\n";
    }
}

void Logger::logStart()
{
    if (writeLog == false)
    {
        QString fileName = QString("log-")+QSysInfo::machineHostName()+QString("-")+QDate::currentDate().toString("yy-MM-dd")+QString("-") \
                +QTime::currentTime().toString("hh-mm-ss")+".txt";
        qDebug() << fileName;
        file.setFileName(fileName);

        if (file.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            qDebug()<<"fileGPS is opened";
            writeLog = true;
        }
        else
        {
            qDebug()<< file.errorString() << " " << file.error();
        }
        QTextStream stream(&file);
//        stream << "lat, NS, long, EW, time, status, posMode\n";
        stream << "data.yaw, data.pitch, \
data.roll, data.X_accel, data.Y_accel, data.Z_accel, \
data.X_rate, data.Y_rate, data.Z_rate, data.X_magn, \
data.Y_magn, data.Z_magn, data.first_qvat, data.second_qvat \
data.third_qvat, data.four_qvat\n";

    }
}

void Logger::logStop()
{
    if (writeLog == true)
    {
        writeLog = false;
        file.close();
        qDebug() << "fileIdle.close";
    }
}
