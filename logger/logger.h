#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include "Parser-BW-AH127C/AH127Cprotocol.h"

class Logger: public QObject
{
    Q_OBJECT
public:
    Logger(QObject *parent = nullptr);
public slots:
    void logTick(DataFromAH127C data);
    void logStart();
    void logStop();
protected:
    bool writeLog = false;
    QFile file;
};

#endif // LOGGER_H
