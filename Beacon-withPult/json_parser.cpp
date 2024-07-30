#include "json_parser.h"

Json_parser::Json_parser()
{
    QString val;
    QFile file;

    file.setFileName("settings.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();
    parser(val);

}

void Json_parser::parser(QString val)
{

    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject json = doc.object();
    set.comGPS = json["COM_GPS"].toString();
    set.comHydro = json["COM_HYDRO"].toString();
    set.puwv1_channel_settings = json["PUWV1"].toString();
    set.receiver_ip = json["receiver.ip"].toString();
    set.receiver_port = json["receiver_port"].toInt();
    qDebug() <<"set.receiver_port:: "  <<set.receiver_port ;
    set.sender_ip = json["sender.ip"].toString();
    set.sender_port = json["sender.port"].toInt();
    set.sender_frequency = json["sender.frequency"].toDouble();
    set.mode_package_delivery = json["mode_package_delivery"].toBool();
}
