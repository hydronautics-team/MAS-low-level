#include "cs_rov.h"
#include <QDebug>
#include <parserUWB/calibration.h>
#include "kx_pult/kx_protocol.h"
#include "kx_pult/qkx_coeffs.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

double X[2000][2];

CS_ROV::CS_ROV(QObject *parent)
{
    realLaunchMode = JSON_init();
    if (realLaunchMode == LaunchMode::REAL_AUV) {
        //передача K
        Qkx_coeffs* kProtocol = new Qkx_coeffs(ConfigFile, KI);
    }
    if (realLaunchMode == LaunchMode::MODEL) {
        Qkx_coeffs* kProtocol = new Qkx_coeffs(ConfigFile, KI_MODEL);
    }
    //передача X
    x_protocol* xProtocol = new x_protocol(ConfigFile, XI, X);

    if (realLaunchMode == LaunchMode::REAL_AUV) {

        AH127C = new AH127Cprotocol("ttyUSB0");  //ttyUSB1
        qDebug() << "Инициализировал БСО";
//        logger.logStart();
//        connect(AH127C, &AH127Cprotocol::updateData, &logger, &Logger::logTick);

        QSettings settings("settings/settings.ini", QSettings::IniFormat);
        settings.beginGroup("Port");
        QString port = settings.value("portname", "/dev/tty.usbserial-AK06UI59").toString();
        qint32 baudrate = settings.value("baudrate", 38400).toInt();
        settings.endGroup();

        vmaProtocol = new VMA_controller(port, baudrate);
        vmaProtocol->moveToThread(&vmaThread);
        QObject::connect(&vmaThread, &QThread::started, vmaProtocol, &VMA_controller::start);
        vmaThread.start();

        trUWB = new UWB::TrilatUWB(this, auvProtocol);
        QString port2 ="ttyUSB1";
        qint32 baudrate2 = 115200;
        prUWB = new UWB::ProtocolUWB (port2, baudrate2);
        prUWB->moveToThread(&uwbThread);
        QObject::connect(&uwbThread, &QThread::started, prUWB, &UWB::ProtocolUWB::start);
        uwbThread.start();

        //ps = new MS5837();

        Calibration *calib = new Calibration(prUWB);
       //может быть расскоментирована только одна из трех строк, или калибровка, или uwb модуль, или гидроакустика
//        QObject::connect(prUWB, &UWB::ProtocolUWB::renewMSG, trUWB, &UWB::TrilatUWB::distanceCalc, Qt::BlockingQueuedConnection);
//        QObject::connect(prUWB, &UWB::ProtocolUWB::renew, calib, &Calibration::newCalibration, Qt::BlockingQueuedConnection);
        hydro = new Hydroacoustics("ttyUSB0");
        connect(&timerRound,&QTimer::timeout, hydro, &Hydroacoustics::modeRound);
        timerRound.start(4000);
        QObject::connect(hydro, &Hydroacoustics::updateData, this, &CS_ROV::updateDataUWB);

        //управление питанием
        wiringPiSetup () ;
        pinMode (27, OUTPUT) ;
        pinMode (28, OUTPUT) ;
        digitalWrite (27, LOW) ;
        digitalWrite (28, LOW) ;
    }

    auvProtocol = new ControlSystem::PC_Protocol(ConfigFile,"agent");
    qDebug() << "-----start exchange-----";
    auvProtocol->startExchange();

    connect(&timer, &QTimer::timeout, this, &CS_ROV::tick);

    timer.start(20);
    timeRegulator.start();

    X[91][0]=X[91][1]=0; //нулевые НУ для интегрирования угловой скорости и нахождения угла курса
    X[92][0]=X[92][1]=0; //нулевые НУ для интегрирования угловой скорости и нахождения угла дифферента
    X[609][0]=X[609][1]=0; //нулевые НУ для дифференцирования контура глубины
}

void CS_ROV::tick()
{
    readDataFromPult();
    readDataFromSensors();
    calibration();
    regulators();
    BFS_DRK(X[101][0], X[102][0], X[103][0] , X[104][0], X[105][0], X[106][0]);
    writeDataToVMA();
    writeDataToPult();

    ms++;
        if (ms>=100-7)
        {
            ms = 0;
            timer_power_power();
        }
}

LaunchMode CS_ROV::JSON_init()
{
    QString configName = "agent_settings.json";
    LaunchMode launchMode;
    QFile file(configName);
    if (!file.exists()) {
        qDebug() << "No config file : " << configName;
        launchMode = LaunchMode::MODEL;
    } else {
        qDebug() << "Config file opened";
        file.open(QIODevice::ReadOnly);
        QJsonDocument doc = QJsonDocument().fromJson(file.readAll());
        QJsonObject confObject = doc.object();
        QString mode = confObject["launch_mode"].toString();
        if (mode == "model") {
            launchMode = LaunchMode::MODEL;
            qDebug() << "model launch mode";
        } else if (mode == "realAUV") {
            launchMode = LaunchMode::REAL_AUV;
            qDebug() << "real AUV launch mode";
        } else {
            qDebug() << "unknown launch mode";
            launchMode = LaunchMode::MODEL;
        }
    }
    return launchMode;
}

void CS_ROV::integrate(double &input, double &output, double &prevOutput, double dt) {
    output = prevOutput + dt*input;
    prevOutput = output;
}

void CS_ROV::resetValues()
{
    vmaProtocol->setValues(0, 0, 0, 0, 0, 0);
}

void CS_ROV::updateDataUWB(uWave uwvave)
{
    X[541][0] = uwvave.puwv3.distanceID1;
    X[542][0] = uwvave.puwv3.distanceID2;
    X[543][0] = uwvave.puwv3.distanceID3;
    X[551][0] = uwvave.puwv3.distanceIDx;
    X[552][0] = uwvave.puwv3.distanceIDy;
}

float CS_ROV::saturation(float input, float max, float min)
{
    if (input>= max) return max;
    else if (input <=min) return min;
    else return input;
}

void CS_ROV::processDesiredValuesAutomatiz(double inputFromRUD, double &output, double &prev_output,
                                           double scaleK, bool flagLimit, double maxValue, double dt) {
    double inputScaled = inputFromRUD*scaleK;
    integrate(inputScaled,output,prev_output,dt);
    if (flagLimit){
        saturation(output,maxValue,-maxValue);
    }
}

double CS_ROV::yawErrorCalculation(float yawDesiredDeg, float yawCurrentDeg)
{
    double l0 =0, l2 =0;
    double Krad = M_PI/180.0;
    double Kdeg = 180/M_PI;
    double desiredPsi = yawDesiredDeg*Krad;
    double currentPsi = yawCurrentDeg*Krad;
    l0=cos(desiredPsi/2)*cos(currentPsi/2)+sin(desiredPsi/2)*sin(currentPsi/2);
    l2=cos(desiredPsi/2)*sin(currentPsi/2)-cos(currentPsi/2)*sin(desiredPsi/2);
    if (fabs(l0)>1) l0=sign(l0)*1;
    if (l0<0) l0*=-1;
    else l2 *=-1;
    double temp = 2*acos(l0);
    double temp_deg = 2*acos(l0)*Kdeg;
    double temp_deg_sign = 2*acos(l0)*sign(l2)*Kdeg;
    return temp_deg_sign;

}

int CS_ROV::sign(double input)
{
    if (input>=0) return 1;
    else return -1;
}

void CS_ROV::readDataFromPult()
{
    X[51][0] = auvProtocol->rec_data.controlData.yaw;
    X[52][0] = auvProtocol->rec_data.controlData.pitch;
    X[53][0] = auvProtocol->rec_data.controlData.roll;
    X[54][0] = auvProtocol->rec_data.controlData.march;
    X[55][0] = auvProtocol->rec_data.controlData.lag;
    X[56][0] = auvProtocol->rec_data.controlData.depth;

//    qDebug() << "given yaw" << auvProtocol->rec_data.controlData.yaw;
//    qDebug() << "given pitch" << auvProtocol->rec_data.controlData.pitch;
//    qDebug() << "given roll" << auvProtocol->rec_data.controlData.roll;
//    qDebug() << "given march" << auvProtocol->rec_data.controlData.march;
//    qDebug() << "given lag" << auvProtocol->rec_data.controlData.lag;
//    qDebug() << "given depth" << auvProtocol->rec_data.controlData.depth;
//    qDebug() << "mode" << uint8_t(auvProtocol->rec_data.cSMode);
//    qDebug() << "beacon_x" << auvProtocol->rec_data.pultUWB.beacon_x;
//    qDebug() << "beacon_y" << auvProtocol->rec_data.pultUWB.beacon_y;
//    qDebug() << "given closed yaw" << auvProtocol->rec_data.controlContoursFlags.yaw;
//    qDebug() << "given closed pitch" << auvProtocol->rec_data.controlContoursFlags.pitch;
//    qDebug() << "given closed roll" << auvProtocol->rec_data.controlContoursFlags.roll;
//    qDebug() << "given closed march" << auvProtocol->rec_data.controlContoursFlags.march;
//    qDebug() << "given closed lag" << auvProtocol->rec_data.controlContoursFlags.lag;
//    qDebug() << "given closed depth" << auvProtocol->rec_data.controlContoursFlags.depth;
//    qDebug() << "modeAUV_selection" << auvProtocol->rec_data.modeAUV_selection;
//    qDebug() << "power_Mode" << uint8_t(auvProtocol->rec_data.pMode);
//    qDebug() << "initCalibration" << uint8_t(auvProtocol->rec_data.flagAH127C_pult.initCalibration);
//    qDebug() << "saveCalibration" << uint8_t(auvProtocol->rec_data.flagAH127C_pult.saveCalibration);
//    qDebug() << "checksum" << uint8_t(auvProtocol->rec_data.checksum);
//    qDebug() << "ID_mission_AUV" << uint8_t(auvProtocol->rec_data.ID_mission_AUV);
//    qDebug() << "missionControl" << uint8_t(auvProtocol->rec_data.missionControl);

    if (auvProtocol->rec_data.cSMode == e_CSMode::MODE_AUTOMATED) qDebug() << "Я в АВТОМАТИЗИРОВАННОМ!";
    if (auvProtocol->rec_data.cSMode == e_CSMode::MODE_MANUAL) qDebug() << "Я в РУЧНОМ!";
    if (auvProtocol->rec_data.cSMode == e_CSMode::MODE_AUTOMATIC) qDebug() << "Я в АВТОМАТИЧЕСКОМ";
    if (auvProtocol->rec_data.modeAUV_selection == 1) setModellingFlag(true);
    else setModellingFlag(false);
}

void CS_ROV::readDataFromSensors()
{
    qDebug() << " я в readDataFromSensors";
    if (modellingFlag == 0 && realLaunchMode == LaunchMode::REAL_AUV) {
        //kx-pult
        X[61][0] = AH127C->data.yaw;
        X[62][0] = AH127C->data.pitch;
        X[63][0] = AH127C->data.roll;
        qDebug() << "AH127C->data.yaw" << X[61][0];
        qDebug() << "AH127C->data.pitch" << X[62][0];
        qDebug() << "AH127C->data.roll" << X[63][0];

        X[64][0] = AH127C->data.X_accel;
        X[65][0] = AH127C->data.Y_accel;
        X[66][0] = AH127C->data.Z_accel;

        X[67][0] = AH127C->data.X_rate;
        X[68][0] = AH127C->data.Y_rate;
        X[69][0] = AH127C->data.Z_rate+K[69]-drewYaw;

        X[70][0] = AH127C->data.X_magn;
        X[71][0] = AH127C->data.Y_magn;
        X[72][0] = AH127C->data.Z_magn;

        X[73][0] = AH127C->data.first_qvat;
        X[74][0] = AH127C->data.second_qvat;
        X[75][0] = AH127C->data.third_qvat;
        X[76][0] = AH127C->data.four_qvat;

//        X[609][0] = ps->depth();
//        qDebug() <<"ps->depth: " << ps->depth();

    } else {
        X[61][0] = X[18][0];
        X[62][0] = X[17][0];
        X[63][0] = X[16][0];

        X[67][0] = X[10][0];
        X[68][0] = X[11][0];
        X[69][0] = X[12][0];
    }
}

void CS_ROV::calibration() {

    if (modellingFlag == 0 && realLaunchMode == LaunchMode::REAL_AUV) {
        if (auvProtocol->rec_data.flagAH127C_pult.initCalibration == true) { //начать калибровку   auvProtocol->rec_data.flagAH127C_pult.initCalibration

            AH127C->flag_start_cal = 1;
            char cmd_rezhim_otveta[6]; //перейти в режим ответа
            cmd_rezhim_otveta[0] = 0x77;
            cmd_rezhim_otveta[1] = 0x05;
            cmd_rezhim_otveta[2] = 0x00;
            cmd_rezhim_otveta[3] = 0x0C;
            cmd_rezhim_otveta[4] = 0x00;
            cmd_rezhim_otveta[5] = 0x11;
            AH127C->m_port.write(cmd_rezhim_otveta, 6);
            AH127C->m_port.waitForBytesWritten();

            char cmd_compas_1[5]; //начать калибровку
            cmd_compas_1[0] = 0x77;
            cmd_compas_1[1] = 0x04;
            cmd_compas_1[2] = 0x00;
            cmd_compas_1[3] = 0x11;
            cmd_compas_1[4] = 0x15;
            AH127C->m_port.write(cmd_compas_1, 5);
            AH127C->m_port.waitForBytesWritten();
        }
            auvProtocol->send_data.flagAH127C_bort.startCalibration = AH127C->flag_calibration_start;

        if (auvProtocol->rec_data.flagAH127C_pult.saveCalibration == true) { //auvProtocol->rec_data.flagAH127C_pult.saveCalibration

            AH127C->flag_finish_cal = 1;
            char cmd_compas_2[5]; //завершить калибровку
            cmd_compas_2[0] = 0x77;
            cmd_compas_2[1] = 0x04;
            cmd_compas_2[2] = 0x00;
            cmd_compas_2[3] = 0x12;
            cmd_compas_2[4] = 0x16;
            AH127C->m_port.write(cmd_compas_2, 5);
            AH127C->m_port.waitForBytesWritten();
        }
            auvProtocol->send_data.flagAH127C_bort.endCalibration = AH127C->flag_calibration_end;
    }
}

void CS_ROV::alternative_yaw_calculation(float dt)
{
    if (modellingFlag == 0 && realLaunchMode == LaunchMode::REAL_AUV) {
        X[170][0] = X[70][0] + K[70]; //Mx с учетом коррекции
        X[171][0] = X[71][0] + K[71]; //My с учетом коррекции
        //X[172][0]=X[72][0] + K[72];
        X[172][0] = X[72][0] + sin(0.5*X[63][0]/57.3)*K[72]; //Mz с учетом коррекции

        double teta = X[62][0]*M_PI/180; double gamma = X[63][0]*M_PI/180;
        X[176][0] = teta;
        X[177][0] = gamma;
        A[0][0] = cos(teta); A[0][1] = sin(teta)*sin(gamma); A[0][2] = -sin(teta)*cos(gamma);
        A[1][0] = 0; A[1][1] = cos(gamma); A[1][2] = sin(gamma);
        A[2][0] = sin(teta); A[2][1] = -sin(gamma)*cos(teta); A[2][2] = cos(teta)*cos(gamma);

        X[300][0] = I[0] = A[0][0]*X[170][0] + A[0][1]*X[171][0] + A[0][2]*X[172][0];
        X[400][0] = I[1] = A[1][0]*X[170][0] + A[1][1]*X[171][0] + A[1][2]*X[172][0];
        X[500][0] = I[2] = A[2][0]*X[170][0] + A[2][1]*X[171][0] + A[2][2]*X[172][0];

        X[174][0] = I[0];
        X[175][0] = I[1];
        X[178][0] = atan2(-I[1],-I[0])*57.3; //почему-то здесь ранее стоял Х75, что странно

        X[79][0] = -1/cos(X[62][0]/57.3)*(-X[69][0]*cos(X[63][0]/57.3)-X[68][0]*sin(X[63][0]/57.3));

        if (!flagYawInit) {
           flagYawInit = true;
           X[91][0] = X[91][1]= X[178][0] + K[178];
           drewYaw = X[69][0];
        }
        //   integrate(X[69][0],X[91][0],X[91][1],dt); //интегрируем показание Z_rate для нахождения текущего угла курса
        integrate(X[79][0],X[91][0],X[91][1],dt); //интегрируем показание Z_rate для нахождения текущего угла курса

    }
}

void CS_ROV::regulators()
{
    float dt = timeRegulator.elapsed()*0.001;   //реальный временной шаг цикла
    timeRegulator.start();
    if (modellingFlag == 0 && realLaunchMode == LaunchMode::REAL_AUV) {
//      integrate(X[69][0],X[91][0],X[91][1],dt); //интегрируем показание Z_rate для нахождения текущего угла курса
        alternative_yaw_calculation(dt);
    }
    if (auvProtocol->rec_data.cSMode == e_CSMode::MODE_MANUAL) { //САУ тогда разомкнута
        if (flag_switch_mode_1 == false) {
            X[5][0] = X[5][1] = 0;
            flag_switch_mode_1 = true;
            flag_switch_mode_2 = false;
            flag_switch_mode_3 = false;
            qDebug() << contour_closure_yaw <<"ручной режим";
        }

        flag_of_mode = 0;
        if (modellingFlag == 1 || realLaunchMode == LaunchMode::MODEL) {
            X[101][0] = K[91]*X[51][0]; //управление по курсу, домножается на коэффициент и передается на ВМА
            X[102][0] = K[92]*X[52][0]; //Uteta
            X[103][0] = K[93]*X[53][0]; //Ugamma
            X[104][0] = K[94]*X[54][0]; //Ux
            X[105][0] = K[95]*X[55][0]; //Uy
            X[106][0] = K[96]*X[56][0]; //Uz
        } else {
            X[101][0] = K[101]*X[51][0]; //Upsi
            X[102][0] = K[102]*X[52][0]; //Uteta
            X[103][0] = K[103]*X[53][0]; //Ugamma
            X[104][0] = K[104]*X[54][0]; //Ux
            X[105][0] = K[105]*X[55][0]; //Uy
            X[106][0] = K[106]*X[56][0]; //Uz
        }

        resetYawChannel();
        resetRollChannel();
        resetPitchChannel();

    } else if (auvProtocol->rec_data.cSMode == e_CSMode::MODE_AUTOMATED) { //САУ в автоматизированном режиме
        flag_of_mode = 1;
        X[104][0] = K[104]*X[54][0]; //Ux  - марш
        X[105][0] = K[105]*X[55][0]; //Uy
        X[106][0] = K[106]*X[56][0]; //Uz

        controlYaw(dt); //контур курса
        controlRoll(dt); //контур крена
//      controlPitch(dt); //контур дифферента
//      controlDepth(dt); //контур глубины

        } else if (auvProtocol->rec_data.cSMode == e_CSMode::MODE_AUTOMATIC) { //САУ в автоматическом режиме
        qDebug() << " mode is auomatic yaw flag is"<<  auvProtocol->rec_data.controlContoursFlags.yaw;
        if (auvProtocol->rec_data.controlContoursFlags.yaw>0) { //замкнут курс
           if (flag_switch_mode_3 == false) {
                X[59][0] = X[58][0]= X[91][0];
                flag_switch_mode_3 = true;
                flag_switch_mode_1 = false;
                flag_switch_mode_2 = false;
                qDebug() << contour_closure_yaw <<"автоматический режим";
           }
           contour_closure_yaw = 1;

           if (auvProtocol->rec_data.ID_mission_AUV != 0 && auvProtocol->rec_data.missionControl != mission_Control::MODE_IDLE) {
               X[59][0] = X[58][0] + X[51][0];
           } else {
               X[59][0] = X[58][0] = X[91][0];
           }

           if (modellingFlag == 1 || realLaunchMode == LaunchMode::MODEL) {
           X[104][0] = K[94]*X[54][0];
           } else {
           X[104][0] = K[104]*X[54][0];  //Ux
           }

           qDebug() << "Ux is : "<<X[104][0] << " " << X[54][0];

           if (modellingFlag == 1 || realLaunchMode == LaunchMode::MODEL) {
                qDebug() << "I in this place";
                qDebug() << "Psi is : "<<X[51][0] << " " << X[111][0];
               X[111][0] = X[51][0] - X[18][0];
           } else {
                qDebug() << "I in this place 2";
               X[111][0] = yawErrorCalculation(X[59][0],X[91][0]); //учет предела работы датчика, пересчет кратчайшего пути
           }
           X[112][0] = X[111][0]*K[111];
           X[113][0] = X[112][0]*K[112];
           X[114][0] = X[114][1] + 0.5*(X[113][0] + X[113][1])*dt; //выходное значение интегратора без полок

           if (K[113] != 0){//значит заданы полки
               X[114][0] = saturation(X[114][0],K[113],K[114]); //выходное значение интегратора с полками
           }
           X[114][1] = X[114][0];
           X[113][1] = X[113][0];

           X[116][0] = X[114][0] + X[112][0];
           if (modellingFlag == 1 || realLaunchMode == LaunchMode::MODEL) {
               aperiodicFilter(X[12][0],X[401][0],X[401][1],K[402],K[403],dt);
           } else {
               aperiodicFilter(X[69][0],X[401][0],X[401][1],K[402],K[403],dt);
           }
           X[121][0] = X[401][0]*K[118];

           X[119][0] = X[59][0]*K[119];
           X[117][0] = X[116][0] - X[121][0] + X[119][0];
           X[118][0] = saturation(X[117][0],K[116],-K[116]);
           X[101][0] = X[118][0]*K[100];
           X[101][0] = X[101][0] + sign(X[101][0])*K[401];        }
        else {
            if (modellingFlag == 1 || realLaunchMode == LaunchMode::MODEL) {
                X[104][0] = K[94]*X[54][0];
            } else {
                X[104][0] = K[104]*X[54][0];
            }
            if (modellingFlag == 1 || realLaunchMode == LaunchMode::MODEL) {
                X[101][0] = K[91]*X[51][0];
            } else {
                X[101][0] = K[101]*X[51][0];
            }
            resetYawChannel();
            X[101][0] = saturation(X[117][0],K[116],-K[116]);
        }
    }
}

void CS_ROV::controlYaw(double dt)
{
    if (auvProtocol->rec_data.controlContoursFlags.yaw>0) { //замкнут курс
       if (flag_switch_mode_2 == false) {
            X[5][1]=X[91][0];
            X[5][0] = 0;
            flag_switch_mode_2 = true;
            flag_switch_mode_1 = false;
            flag_switch_mode_3 = false;
            qDebug() << contour_closure_yaw <<"автоматизированный режим";
       }
       contour_closure_yaw = 1;

       processDesiredValuesAutomatiz(X[51][0],X[5][0],X[5][1],K[2]); //пересчет рукоятки в автоматизированном режиме
       if (modellingFlag == 1 || realLaunchMode == LaunchMode::MODEL) {
           X[111][0] = X[5][0] - X[18][0];
       } else {
           X[111][0] = yawErrorCalculation(X[5][0],X[91][0]); //учет предела работы датчика, пересчет кратчайшего пути
           }
       X[112][0] = X[111][0]*K[111];
       X[113][0] = X[112][0]*K[112];
       X[114][0] = X[114][1] + 0.5*(X[113][0] + X[113][1])*dt; //выходное значение интегратора без полок

       if (K[113] != 0){//значит заданы полки
           X[114][0] = saturation(X[114][0],K[113],K[114]); //выходное значение интегратора с полками
       }
       X[114][1] = X[114][0];
       X[113][1] = X[113][0];

       X[116][0] = X[114][0] + X[112][0];
       if (modellingFlag == 1 || realLaunchMode == LaunchMode::MODEL) {
           aperiodicFilter(X[12][0],X[401][0],X[401][1],K[402],K[403],dt);
       } else {
           aperiodicFilter(X[79][0],X[401][0],X[401][1],K[402],K[403],dt);
       }
       X[121][0] = X[401][0]*K[118];

       X[119][0] = X[51][0]*K[119];
       X[117][0] = X[116][0] - X[121][0] + X[119][0];

    } else {
        X[117][0] = K[101]*X[51][0]; //Upsi
        resetYawChannel();
    }
    X[118][0] = saturation(X[117][0],K[116],-K[116]);
    X[101][0] = X[118][0]*K[100];
    //X[101][0] = X[101][0] + sign(X[101][0])*K[401];

}

void CS_ROV::controlPitch(double dt)
{
//    if (auvProtocol->rec_data.controlContoursFlags.pitch>0) { //замкнут дифферент
//        processDesiredValuesAutomatiz(X[52][0],X[6][0],X[6][1],K[2]); //пересчет рукоятки в автоматизированном режиме
//        X[311][0] = X[6][0] - X[62][0];
//        X[312][0] = X[311][0]*K[311];
//        X[313][0] = X[312][0]*K[312];
//        X[314][0] = X[314][1] + 0.5*(X[313][0] + X[313][1])*dt; //выходное значение интегратора без полок

//        if (K[313] != 0){//значит заданы полки
//            X[314][0] = saturation(X[314][0],K[313],K[314]); //выходное значение интегратора с полками
//         }
//         X[314][1] = X[314][0];
//         X[313][1] = X[313][0];

//         X[316][0] = X[314][0] + X[312][0];

//         X[321][0] = X[68][0]*K[318];
//         X[319][0] = X[52][0]*K[319];
//         X[317][0] = X[116][0] - X[121][0] + X[119][0];
//   } else {
//         X[317][0] = K[301]*X[52][0]; //Upsi
//         resetRollChannel();
//   }
//   X[318][0] = saturation(X[317][0],K[316],-K[316]);
//   X[301][0] = X[318][0]*K[300];
}

void CS_ROV::controlRoll(double dt)
{
    if (auvProtocol->rec_data.controlContoursFlags.roll>0) {
        contour_closure_roll = 1;

        processDesiredValuesAutomatiz(X[53][0],X[800][0],X[800][1],K[800]);
        X[801][0] = X[800][0] - X[63][0];

        X[802][0] = X[801][0] * K[801];
        X[803][0] = X[802][0] * K[802];
        integrate(X[803][0],X[804][0],X[804][1],dt);
        if (K[803]>0) {
            X[804][1] = saturation(X[804][0],K[804],-K[804]);
            X[804][0] = saturation(X[804][0],K[804],-K[804]);
        }
        X[814][0] = K[814] + sin((X[800][0]-K[816])/57.3) * K[815];

        X[806][0] = X[802][0] + X[804][0];
        X[805][0] = X[53][0] * K[805];

        X[807][0] = X[806][0] + X[805][0]+ X[814][0];

        //speed loop
        aperiodicFilter(X[67][0],X[808][0],X[808][1],1,K[808],dt);
        X[809][0] = X[808][0] * K[809];
        X[810][0] = X[807][0] - X[809][0] + K[817]*X[56][0];

    } else {
        X[810][0] = X[53][0] * K[103];
        resetRollChannel();
    }
    X[811][0] = saturation(X[810][0],K[810],-K[810]);
    X[103][0] = K[811] * X[811][0];
}

void CS_ROV::controlDepth(double dt)
{
//   if (auvProtocol->rec_data.controlContoursFlags.depth > 0) { //замкнута глубина
//       contour_closure_depth = 1;
//       if (modellingFlag == 1 || realLaunchMode == LaunchMode::MODEL) {
//          X[601][0] = X[56][0] - X[21][0];
//       } else {
//       X[601][0] = X[56][0] - X[609][0];
//       }
//       X[602][0] = X[601][0]*K[600]; //Усиление ошибки по положению
//       X[605][0] = X[602][0]; //Интегрирующую составляющую пропустили

//       if (modellingFlag == 1 || realLaunchMode == LaunchMode::MODEL) {
//           X[611][0] = X[30][0]*K[606];
//       } else {
//       X[610][0] = (X[609][0]-X[609][1])/1; // Дифференцирование
//       X[611][0] = X[610][0]*K[606]; // Усиление обратной связи по скорости
//       }

//       X[613][0] = X[56][0]*K[605]; // Усиление возмущающего воздействия по скорости
//       X[606][0] = X[605][0] - X[611][0] + X[613][0]; // Получение ошибки по скорости

//       X[609][1] = X[609][0];
//   } else {
//       X[606][0] = K[96]*X[56][0]; //Uz
//       resetDepthChannel();
//   }
//   X[607][0] = saturation(X[606][0],K[604],-K[604]); // Насыщение в контуре скорости
//   X[106][0] = X[607][0]*K[607]; // Усиление ошибки по скорости
}

void CS_ROV::resetYawChannel()
{
    X[5][0] = X[5][1] = 0;
    X[114][1] = X[114][0] = 0;
}

void CS_ROV::resetRollChannel()
{
    //X[800][0] = X[800][1] = X[63][0];
    X[800][0] = X[800][1] = 0;
    X[804][0] = X[804][1] = 0;
}

void CS_ROV::resetPitchChannel() {
     X[6][0] = X[6][1] = 0;
     X[314][0] = X[314][1] = 0;
}

void CS_ROV::resetDepthChannel() {
     X[604][0] = X[604][1] = 0;
}

void CS_ROV::BFS_DRK(double Upsi, double Uteta, double Ugamma, double Ux, double Uy, double Uz)
{
    X[110][0] = (K[10]*Ux + K[11]*Uy + K[12]*Uz + K[13]*Ugamma + K[14]*Uteta + K[15]*Upsi)*K[16];//U1
    X[120][0] = (K[20]*Ux + K[21]*Uy + K[22]*Uz + K[23]*Ugamma + K[24]*Uteta + K[25]*Upsi)*K[26];//U2
    X[130][0] = (K[30]*Ux + K[31]*Uy + K[32]*Uz + K[33]*Ugamma + K[34]*Uteta + K[35]*Upsi)*K[36];//U3
    X[140][0] = (K[40]*Ux + K[41]*Uy + K[42]*Uz + K[43]*Ugamma + K[44]*Uteta + K[45]*Upsi)*K[46];//U4
    X[150][0] = (K[50]*Ux + K[51]*Uy + K[52]*Uz + K[53]*Ugamma + K[54]*Uteta + K[55]*Upsi)*K[56];//U5
    X[160][0] = (K[60]*Ux + K[61]*Uy + K[62]*Uz + K[63]*Ugamma + K[64]*Uteta + K[65]*Upsi)*K[66];//U6
    X[211][0] = limit(X[110][0],K[200]);
    X[221][0] = limit(X[120][0],K[200]);
    X[231][0] = limit(X[130][0],K[200]);
    X[241][0] = limit(X[140][0],K[200]);
    X[251][0] = limit(X[150][0],K[200]);
    X[261][0] = limit(X[160][0],K[200]);

}

void CS_ROV::writeDataToPult()
{
    auvProtocol->send_data.headerSwap.senderID;
    auvProtocol->send_data.headerSwap.receiverID;
    auvProtocol->send_data.headerSwap.msgSize;

    auvProtocol->send_data.auvData.modeReal = flag_of_mode;
    auvProtocol->send_data.auvData.controlReal.yaw = contour_closure_yaw;
    auvProtocol->send_data.auvData.controlReal.pitch = contour_closure_pitch;
    auvProtocol->send_data.auvData.controlReal.roll = contour_closure_roll;
    auvProtocol->send_data.auvData.controlReal.march = contour_closure_march;
    auvProtocol->send_data.auvData.controlReal.depth = contour_closure_depth;
    auvProtocol->send_data.auvData.controlReal.lag = contour_closure_lag;
    auvProtocol->send_data.auvData.modeAUV_Real = modellingFlag;
    auvProtocol->send_data.auvData.ControlDataReal.yaw;
    auvProtocol->send_data.auvData.ControlDataReal.pitch;
    auvProtocol->send_data.auvData.ControlDataReal.roll;
    auvProtocol->send_data.auvData.ControlDataReal.march;
    auvProtocol->send_data.auvData.ControlDataReal.depth;
    auvProtocol->send_data.auvData.ControlDataReal.lag;
    auvProtocol->send_data.auvData.signalVMA_real;
    auvProtocol->send_data.ID_mission = auvProtocol->rec_data.ID_mission_AUV;

    if (auvProtocol->rec_data.missionControl == mission_Control::MODE_START) {
            auvProtocol->send_data.missionStatus = mission_Status::MODE_RUNNING;
    }

    if (auvProtocol->rec_data.missionControl == mission_Control::MODE_START) {
            auvProtocol->send_data.missionStatus = mission_Status::MODE_RUNNING;
    }

    if (auvProtocol->rec_data.missionControl == mission_Control::MODE_COMPLETE) {
            auvProtocol->send_data.missionStatus = mission_Status::MODE_IDLE;
    }

    if (modellingFlag == 1 || realLaunchMode == LaunchMode::MODEL) {
        auvProtocol->send_data.dataAH127C.yaw = X[18][0]; //61 - это с датчика
        auvProtocol->send_data.dataAH127C.pitch = X[17][0];
        auvProtocol->send_data.dataAH127C.roll = X[16][0];
        auvProtocol->send_data.dataAH127C.X_rate = X[10][0];
        auvProtocol->send_data.dataAH127C.Y_rate = X[11][0];
        auvProtocol->send_data.dataAH127C.Z_rate = X[12][0];

        auvProtocol->send_data.dataUWB.locationX = X[19][0];
        auvProtocol->send_data.dataUWB.locationY = X[20][0];

        auvProtocol->send_data.auvData.signalVMA_real.VMA1 = X[211][0];
        auvProtocol->send_data.auvData.signalVMA_real.VMA2 = X[221][0];
        auvProtocol->send_data.auvData.signalVMA_real.VMA3 = X[231][0];
        auvProtocol->send_data.auvData.signalVMA_real.VMA4 = X[241][0];
        auvProtocol->send_data.auvData.signalVMA_real.VMA5 = X[251][0];
        auvProtocol->send_data.auvData.signalVMA_real.VMA6 = X[261][0];

    } else {
        auvProtocol->send_data.dataAH127C.yaw = X[91][0];
        qDebug() << "X[91][0] "<< X[91][0];
        qDebug() << "X[62][0] "<< X[62][0];
        qDebug() << "X[63][0] "<< X[63][0];
        auvProtocol->send_data.dataAH127C.pitch = X[62][0];
        auvProtocol->send_data.dataAH127C.roll = X[63][0];
        auvProtocol->send_data.dataAH127C.X_accel = X[64][0];
        auvProtocol->send_data.dataAH127C.Y_accel = X[65][0];
        auvProtocol->send_data.dataAH127C.Z_accel = X[66][0];
        auvProtocol->send_data.dataAH127C.X_rate = X[67][0];
        auvProtocol->send_data.dataAH127C.Y_rate = X[68][0];
        auvProtocol->send_data.dataAH127C.Z_rate = X[69][0];
        auvProtocol->send_data.dataAH127C.X_magn = X[70][0];
        auvProtocol->send_data.dataAH127C.Y_magn = X[71][0];
        auvProtocol->send_data.dataAH127C.Z_magn = X[72][0];
        auvProtocol->send_data.dataAH127C.quat[0] = X[73][0];
        auvProtocol->send_data.dataAH127C.quat[1] = X[74][0];
        auvProtocol->send_data.dataAH127C.quat[2] = X[75][0];
        auvProtocol->send_data.dataAH127C.quat[3] = X[76][0];

        auvProtocol->send_data.dataUWB.connection_field = prUWB->msg.connection_field;
        auvProtocol->send_data.dataUWB.error_code = prUWB->msg.error_code;
        auvProtocol->send_data.dataUWB.distanceToBeacon[0] = X[541][0];
        auvProtocol->send_data.dataUWB.distanceToBeacon[1] = X[542][0];
        auvProtocol->send_data.dataUWB.distanceToBeacon[2] = X[543][0];
        auvProtocol->send_data.dataUWB.locationX = X[551][0];
        auvProtocol->send_data.dataUWB.locationY = X[552][0];
        qDebug() << "X[551][0]" << X[551][0];
        qDebug() << "X[552][0]" << X[552][0];
        auvProtocol->send_data.auvData.signalVMA_real.VMA1 = X[80][0];
        auvProtocol->send_data.auvData.signalVMA_real.VMA2 = X[81][0];
        auvProtocol->send_data.auvData.signalVMA_real.VMA3 = X[82][0];
        auvProtocol->send_data.auvData.signalVMA_real.VMA4 = X[83][0];
        auvProtocol->send_data.auvData.signalVMA_real.VMA5 = X[84][0];
        auvProtocol->send_data.auvData.signalVMA_real.VMA6 = X[85][0];
    }
//    qDebug()<<"additional auvProtocol->send_data.dataUWB.locationX  "<<auvProtocol->send_data.dataUWB.locationX;
}

void CS_ROV:: timer_power_power()
  {
      if (auvProtocol->rec_data.pMode == power_Mode::MODE_2){
            qDebug() << "power_Mode::MODE_2";
            digitalWrite (27, LOW) ;
            digitalWrite (28, LOW) ;
      }
      else if (auvProtocol->rec_data.pMode == power_Mode::MODE_3){
          digitalWrite (27, LOW) ;
          digitalWrite (28, HIGH) ;
          qDebug() << "power_Mode::MODE_3";
      }
      else if (auvProtocol->rec_data.pMode == power_Mode::MODE_4){
          digitalWrite (27, HIGH) ;
          digitalWrite (28, LOW) ;
          qDebug() << "power_Mode::MODE_4";
      }
      else if (auvProtocol->rec_data.pMode == power_Mode::MODE_5){
          digitalWrite (27, HIGH) ;
          digitalWrite (28, HIGH) ;
          qDebug() << "power_Mode::MODE_5";
      }
      else {
          qDebug() << "power_Mode:: WRONG";
      }
  }

void CS_ROV::setModellingFlag(bool flag)
{
    if (modellingFlag!=flag) {
        if (modellingFlag == false) resetValues();
        modellingFlag = flag;
    }
}

void CS_ROV::aperiodicFilter(double &input, double &output, double &prevOutput, double K, double T, double dt)
{
    if (T !=0) output = prevOutput + dt*(1/T)*(input*K-prevOutput);
    else output = K*input;
    prevOutput = output;
}

void CS_ROV::writeDataToVMA()
{
    if (modellingFlag == 1 || realLaunchMode == LaunchMode::MODEL) {//режим модели
        model.tick(X[211][0], X[221][0], X[231][0], X[241][0], X[251][0], X[261][0], 0.01);
    }
    else {
      vmaProtocol->setValues(X[211][0], X[221][0], X[231][0], X[241][0], X[251][0], X[261][0]);
    }
}
