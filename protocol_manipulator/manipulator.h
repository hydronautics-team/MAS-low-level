#pragma once
#include <QtSerialPort/QSerialPort>
#include <QByteArray>
#include <QObject>
#include <QMutex>
#include <QTimer>
#include <QThread>
#include <QSettings>
#include "math.h"

//для использования kx-pult
extern double X[2000][2];
extern QVector<double> K;


class Manipulator_controller: public QObject {
    Q_OBJECT

public:
    Manipulator_controller(const QString& portName,
                  const qint32 baud,
                  const QSerialPort::DataBits data = QSerialPort::Data8,
                  const QSerialPort::StopBits stop = QSerialPort::OneStop,
                  const QSerialPort::Parity parity = QSerialPort::NoParity,
                  const QSerialPort::FlowControl flow = QSerialPort::NoFlowControl,
                  QObject* parent = nullptr);
    void settings_mp(QObject *parent);

public slots:
    void start();
    void stop();
    void setValues(const int command);
signals:
//    void started();
//    void finished();

private:
    QSerialPort* mSerialPort = new QSerialPort(this);
    QString mPortName;
    qint32 mBaud;
    QSerialPort::DataBits mDataBits;
    QSerialPort::StopBits mStopBits;
    QSerialPort::Parity mParity;
    QSerialPort::FlowControl mFlow;
    QTimer mTimer;
    char manipulator_command;
    bool isConnected = false;
    mutable QMutex mGuard;
    static constexpr int REQUEST_TIME = 100;
    static constexpr int PACKET_SIZE = 18;
    bool sendData();

private slots:
    void tick();
};
