#include "manipulator.h"
#include <QSerialPort>
#include <QIODevice>
#include <QDebug>

Manipulator_controller::Manipulator_controller(const QString& portName,
                             const qint32 baud,
                             const QSerialPort::DataBits data,
                             const QSerialPort::StopBits stop,
                             const QSerialPort::Parity parity,
                             const QSerialPort::FlowControl flow,
                             QObject* parent) :
    mPortName(portName),
    mBaud(baud),
    mDataBits(data),
    mStopBits(stop),
    mParity(parity),
    mFlow(flow),
    mTimer(this) {
    connect(&mTimer, &QTimer::timeout, this, &Manipulator_controller::tick);
}

void Manipulator_controller::start() {
    QMutexLocker lock(&mGuard);
    if (mSerialPort->isOpen()) {
        qDebug() << "Duplicate open request";
        return;
    }
    mSerialPort->setPortName(mPortName);
    mSerialPort->setBaudRate(mBaud);
    mSerialPort->setDataBits(mDataBits);
    mSerialPort->setStopBits(mStopBits);
    mSerialPort->setParity(mParity);
    mSerialPort->setFlowControl(mFlow);
    if (!mSerialPort->open(QIODevice::ReadWrite)) {
        qDebug() << "Error connecting to manipulator" << mSerialPort->errorString();
        return;
    }
    mTimer.start(REQUEST_TIME);
    qDebug() << "Port opened";
    isConnected = true;
//    emit started();
    return;
}

void Manipulator_controller::tick() {
    sendData();
}

bool Manipulator_controller::sendData() {
    bool res = true;
    QByteArray packet;

    packet.append((const char)(manipulator_command)); // Добавляем текущую команду в массив

    qint8 bytesWritten = mSerialPort->write(packet);
    if ((bytesWritten != packet.size())) {
        qDebug() << mSerialPort->errorString();
        res = false;
    }
    packet.clear();
    return res;

    //packet.append((const char*)(vmaVector + i), sizeof(qint16));
}

void Manipulator_controller::setValues(const int command)
{
    manipulator_command = command;
}

void Manipulator_controller::stop() {
    QMutexLocker lock(&mGuard);
    if (mSerialPort && mSerialPort->isOpen())
        mSerialPort->close();
//    emit finished();
}
