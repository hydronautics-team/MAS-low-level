#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include "nmea0183.h"
#include "hydroacoustics.h"

class Logger : public QObject
{
    Q_OBJECT
public:
    Logger(QObject *parent = nullptr);
    GPS gpsS;
public slots:
    void logTickGPS(GPS gps);
    void logTickIdle(uWave uwave);
    void logTickRound(uWave uwave);
    void logDirect(uWave uwave);
    void logStartGPS();
    void logStartIdle();
    void logStartRoundR();
    void logStartDirect();
    void logStopGPS();
    void logStopIdle();
    void logStopRoundR();
    void logStopDirect();
protected:
    bool writeLogGPS = false;
    bool writeLogIdle = false;
    bool writelogRoundR = false;
    bool writelogDirect = false;
    QFile fileGPS;
    QFile fileIdle;
    QFile fileRound;
    QFile fileDirect;
};

#endif // LOGGER_H
