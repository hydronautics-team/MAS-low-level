#ifndef NMEA0183_H
#define NMEA0183_H

#include <QObject>
#include <QSerialPort>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <QDate>

#pragma pack(push,1)
struct GLL
{
    float lat = 0;
    char NS = 0;
    float long_ = 0;
    char EW = 0;
//    uint16_t time_h = 0;
//    uint16_t time_m = 0;
//    uint16_t time_s = 0;
//    uint16_t time_ms = 0;
    QTime time;
    char status = 0;
    char posMode = 0;
};

struct RMS
{
    QTime time;
    char status = 0;
    float lat = 0;
    char NS = 0;
    float long_ = 0;
    char EW = 0;
    float spd = 0;
    float cog = 0;
    QDate date;
    float mv;
    char mvEW;
    char posMode;
    int counter = 0;
};

struct GPS
{
    GLL gll;
    RMS rms;
};

#pragma pack(pop)

class NMEA0183 : public QObject
{
    Q_OBJECT
public:
    explicit NMEA0183(QString portName, int baudRate = 115200, QObject *parent = 0);
    void readData();
    GPS gps;
protected:
    QSerialPort gps_port;
    QByteArray gps_buffer;
    QByteArray GPRMC = "GPRMC";
    QByteArray GPVTG = "GPVTG";
    QByteArray GPTXT = "GPTXT";
    QByteArray GPGGA = "GPGGA";
    QByteArray GPGSA = "GPGSA";
    QByteArray GPGSV = "GPGSV";
    QByteArray GPGLL = "GPGLL";
    void parseBuffer();
    void parseGPRMC(QByteArray msg);
    void parseGPVTG(QByteArray msg);
    void parseGPTXT(QByteArray msg);
    void parseGPGGA(QByteArray msg);
    void parseGPGSA(QByteArray msg);
    void parseGPGSV(QByteArray msg);
    void parseGPGLL(QByteArray msg);
    int crc (QByteArray msg);

    QTimer timer;
signals:
    void newMessageDetected(GPS gps);

};

#endif // NMEA0183_H
