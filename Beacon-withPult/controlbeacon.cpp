#include "controlbeacon.h"

//#include "protocol/pc_protocol.h"


ControlBeacon::ControlBeacon(QObject *parent)
{

//    beaconProtocol = new ControlSystem::PC_Protocol();
//    qDebug() << "-----start exchange with Pult";
//    beaconProtocol->startExchange();


//    Json_parser js;
//    qDebug() << "js.set.comHydro: " <<js.set.comHydro;
    hydro = new Hydroacoustics("ttyUSB1");
//    gps = new NMEA0183 (set.comGPS);


    connect(&timerCmd1,&QTimer::timeout,hydro,&Hydroacoustics::sendCmd1);
    connect(&timerCmd2,&QTimer::timeout,hydro,&Hydroacoustics::sendCmd2);
    connect(&timerIdle,&QTimer::timeout,hydro,&Hydroacoustics::modeIdle);
//    connect(this, &ControlBeacon::startDirectConnection, hydro, &Hydroacoustics::modeDirect);
    connect(&timerDirect,&QTimer::timeout, hydro, &Hydroacoustics::modeDirect);
//    connect(this,&ControlBeacon::startRoundR,hydro,&Hydroacoustics::modeRound);
    connect(this,&ControlBeacon::startDirectChannel,hydro,&Hydroacoustics::settingsChannelDirect);
    connect(this,&ControlBeacon::startRoundChannel,hydro,&Hydroacoustics::settingsChannelRound);
    connect(&timerRound,&QTimer::timeout, hydro, &Hydroacoustics::modeRound);
    connect(hydro, &Hydroacoustics::updateData, &logger, &Logger::logTickIdle);
    connect(hydro, &Hydroacoustics::updateData, &logger, &Logger::logDirect);
    connect(hydro, &Hydroacoustics::updateData, &logger, &Logger::logTickRound);
    connect(hydro, &Hydroacoustics::updateData, this, &ControlBeacon::update);



    InitState = new QState();
    Idle = new QState();
    RoundR = new QState();
    DirectConnection = new QState();

    SendCmd1 = new QState(InitState); //состояние внутри InitState
    SendCmd2 = new QState(InitState);

    InitState->setInitialState(SendCmd1);//первое состоние, в котором будет InitState

//    InitState->addTransition(this, &ControlBeacon::initDone, Idle); //переход в Idle по сигналу initDone
    Idle->addTransition(this, &ControlBeacon::startRoundR, RoundR);
    Idle->addTransition(this, &ControlBeacon::startDirectConnection, DirectConnection);
    RoundR->addTransition(this, &ControlBeacon::finishExchange, Idle);
    DirectConnection->addTransition(this,&ControlBeacon::finishExchange,Idle);

    SendCmd1->addTransition(hydro, &Hydroacoustics::cmd1Received,SendCmd2);
    SendCmd2->addTransition(hydro, &Hydroacoustics::initCmd2Done,Idle);

    InitState->assignProperty(this,"m_state",statesMap.value(State::InitState));
    Idle->assignProperty(this,"m_state", "Idle");
    RoundR->assignProperty(this,"m_state","RoundR");
    DirectConnection->assignProperty(this, "m_state","DirectConnection");

    SendCmd1->assignProperty(this,"m_subState","SendCmd1");
    SendCmd2->assignProperty(this,"m_subState","SendCmd2");


    connect(InitState,&QState::entered,this, &ControlBeacon::initStateSlot); //при входе в InitState вызывается initStateSlot
    connect(SendCmd1,&QState::entered,this, [this](){ //входим в состояние SendCmd1 и запускаем таймер
        timerCmd1.start(2000);
    });
    connect(SendCmd1, &QState::exited,this, [this](){ //выходим из состояния SendCmd1 и выключаем таймер
        timerCmd1.stop();
        hydro->stopCounter();
    });
    connect(SendCmd2,&QState::entered,this, [this](){ //входим в состояние SendCmd1 и запускаем таймер
        timerCmd2.start(2000);
    });
    connect(SendCmd2, &QState::exited,this, [this](){ //выходим из состояния SendCmd1 и выключаем таймер
        timerCmd2.stop();
        hydro->stopCounter();
    });
    connect(Idle,&QState::entered,this, [this](){ //входим в состояние SendCmd1 и запускаем таймер
//        logger.logStartIdle();
    });
    connect(Idle, &QState::exited,this, [this](){ //выходим из состояния SendCmd1 и выключаем таймер
//        logger.logStopIdle();
        hydro->stopCounter();
    });
    connect(DirectConnection,&QState::entered,this, [this](){ //входим в состояние SendCmd1 и запускаем таймер
        logger.logStartDirect();
        qDebug() << "timeQuest: " << timeQuest;
        timerDirect.start(timeQuest);

    });
    connect(DirectConnection, &QState::exited,this, [this](){ //выходим из состояния SendCmd1 и выключаем таймер
        logger.logStopDirect();
        hydro->stopCounter();
        timerDirect.stop();
    });
    connect(RoundR,&QState::entered,this, [this](){ //входим в состояние SendCmd1 и запускаем таймер
        logger.logStartRoundR();
//        hydro->timerRound.start(2000);
        qDebug() << "timeQuest: " << timeQuest;
        timerRound.start(timeQuest);
    });
    connect(RoundR, &QState::exited,this, [this](){ //выходим из состояния SendCmd1 и выключаем таймер
        logger.logStopRoundR();
        hydro->stopCounter();
//        hydro->timerRound.stop();
        timerRound.stop();
    });

    machine.addState(InitState);
    machine.addState(Idle);
    machine.addState(RoundR);
    machine.addState(DirectConnection);
    machine.setInitialState(InitState);

    machine.start();

//    connect(InitState, &QState::entered, this, [this](){
//        qDebug()<<"Init state entred";
//        emit initDone();
//    });


    connect(&timer, &QTimer::timeout,this,&ControlBeacon::tick);
    timer.start(dt);

}


void ControlBeacon::tick()
{

  //  hydro->uwave.distance_real = beaconProtocol->rec_data.distance_real;
    if (m_state == statesMap.value(State::Idle))
    {
////       qDebug() << "захожу в тик, тик-тик";
//       if (beaconProtocol->rec_data.mode_select == Mode_select::DIRECT)
//       {
//           chanel.txCh = beaconProtocol->rec_data.mode_data.direct.receiver.id;
//           chanel.rxCh = 0;
//           timeQuest =  beaconProtocol->rec_data.mode_data.direct.dt*1000;
//           emit startDirectChannel(chanel);
//           emit startDirectConnection();

//       }
//       if (beaconProtocol->rec_data.mode_select == Mode_select::ROUND)
       {
//           timeQuest =  beaconProtocol->rec_data.mode_data.round.dt*1000;
           timeQuest = 4000;
          // chRCB.Number = beaconProtocol->rec_data.mode_data.round.countBeacon;
           chRCB.Number = 4;
           chRCB.rxCh = 0;
//           chRCB.txCh1 = beaconProtocol->rec_data.mode_data.round.receiver1.id;
//           qDebug() << " beaconProtocol->rec_data.mode_data.round.receiver1.id:"<<  beaconProtocol->rec_data.mode_data.round.receiver1.id;
//           chRCB.txCh2 = beaconProtocol->rec_data.mode_data.round.receiver2.id;
//           qDebug() << " beaconProtocol->rec_data.mode_data.round.receiver2.id:"<<  beaconProtocol->rec_data.mode_data.round.receiver2.id;
//           chRCB.txCh3 = beaconProtocol->rec_data.mode_data.round.receiver3.id;
//           qDebug() << " beaconProtocol->rec_data.mode_data.round.receiver3.id:"<<  beaconProtocol->rec_data.mode_data.round.receiver3.id;
//           chRCB.txCh4 = beaconProtocol->rec_data.mode_data.round.receiver4.id;
//           qDebug() << " beaconProtocol->rec_data.mode_data.round.receiver4.id:"<<  beaconProtocol->rec_data.mode_data.round.receiver4.id;
           chRCB.txCh1 = 1;
           chRCB.txCh2 = 2;
           chRCB.txCh3 = 3;
           chRCB.txCh4 = 4;
           emit startRoundChannel(chRCB);
           emit startRoundR();
       }
//     if (hydro->salinity != beaconProtocol->rec_data.salinity)
//     {
//         hydro->salinity = beaconProtocol->rec_data.salinity;
//         hydro->sendCmd2();
//     }
    }
  //  else
        if (m_state == "InitState")
    {
        //float timerr = beaconProtocol->rec_data.mode_data.round.dt;
        //if (timerr != timeQuest) emit updateTime(timerr);
    }
    else if (m_state == "RoundR")
    {
        float timerr = beaconProtocol->rec_data.mode_data.round.dt;
        if (timerr != timeQuest) emit updateTime(timerr);
        if ((beaconProtocol->rec_data.mode_select == Mode_select::IDLE) or (beaconProtocol->rec_data.mode_select == Mode_select::DIRECT))
        {
            slotStop();
        }
    }
    else if (m_state == "DirectConnection")
    {
        float timerr = beaconProtocol->rec_data.mode_data.direct.dt;
        if (timerr != timeQuest) emit updateTime(timerr);
        if ((beaconProtocol->rec_data.mode_select == Mode_select::IDLE) or (beaconProtocol->rec_data.mode_select == Mode_select::ROUND))
        {
            slotStop();
        }
    }
}

void ControlBeacon::initStateSlot()
{
//    connect(gps, &NMEA0183::newMessageDetected,
//                     &logger, &Logger::logTickGPS);
//    logger.logStartGPS();
    //УДАЛИТЬ!!!
//    hydro->request_PUWV2(1,1);

}




void ControlBeacon::slotStartDirect(Channel ch)
{
    chanel.txCh = ch.txCh;
    chanel.rxCh = ch.rxCh;
        qDebug() << "chanel.txCh" << chanel.txCh;
        qDebug() << "chanel.rxCh" << chanel.rxCh;
    emit startDirectChannel(chanel);
    emit startDirectConnection();
}

void ControlBeacon::slotStartRound(ChannelRound chW)
{
    chRCB= chW;
    emit startRoundChannel(chRCB);
    emit startRoundR();
}

void ControlBeacon::slotStop()
{
    emit finishExchange();
}

void ControlBeacon::update(uWave uwave)
{
    beaconProtocol->send_data.mode_select;
    beaconProtocol->send_data.depth = hydro->uwave.puwv7.Depth_m;
    qDebug() << "Depth_m: " << beaconProtocol->send_data.depth;
    beaconProtocol->send_data.temperature = hydro->uwave.puwv7.Temperature_C;
    qDebug() << "temperature: " << beaconProtocol->send_data.temperature;
    beaconProtocol->send_data.voltage = hydro->uwave.puwv7.VCC_V;
    qDebug() << "voltage: " <<beaconProtocol->send_data.voltage;
    beaconProtocol->send_data.mode_data.direct.receiver.id = hydro->uwave.puwv3.rcCmdID;
    qDebug() << "puwv3.rcCmdID: " << hydro->uwave.puwv3.rcCmdID;
    beaconProtocol->send_data.mode_data.direct.receiver.nbr_rd = hydro->uwave.puwv3.counterAll;
    qDebug() << "hydro->uwave.puwv3.counterAll: " << hydro->uwave.puwv3.counterAll;
    beaconProtocol->send_data.mode_data.direct.receiver.nbr_td = hydro->uwave.counterACK;
    qDebug() << "hydro->uwave.counterACK: " << hydro->uwave.counterACK;
    if (beaconProtocol->rec_data.mode_data.direct.receiver.id == 1)
    beaconProtocol->send_data.mode_data.direct.receiver.tFlight = hydro->uwave.puwv3.distanceID1;
    if (beaconProtocol->rec_data.mode_data.direct.receiver.id == 2)
    beaconProtocol->send_data.mode_data.direct.receiver.tFlight = hydro->uwave.puwv3.distanceID2;
    if (beaconProtocol->rec_data.mode_data.direct.receiver.id == 3)
    beaconProtocol->send_data.mode_data.direct.receiver.tFlight = hydro->uwave.puwv3.distanceID3;
    if (beaconProtocol->rec_data.mode_data.direct.receiver.id == 4)
    beaconProtocol->send_data.mode_data.direct.receiver.tFlight = hydro->uwave.puwv3.distanceID4;
    qDebug() << "hydro->uwave.puwv3.distance: " << hydro->uwave.puwv3.distance;
    beaconProtocol->send_data.mode_data.round.receiver1.nbr_rd = hydro->uwave.puwv3.counterID1;
    qDebug() << "hydro->uwave.puwv3.counterID1: " << hydro->uwave.puwv3.counterID1;
    beaconProtocol->send_data.mode_data.round.receiver1.nbr_td = hydro->uwave.counterACK1;
    qDebug() << "hydro->uwave.counterACK1: " << hydro->uwave.counterACK1;
    beaconProtocol->send_data.mode_data.round.receiver1.tFlight = hydro->uwave.puwv3.distanceID1;
    qDebug() << "hydro->uwave.puwv3.distanceID1: " << hydro->uwave.puwv3.distanceID1;
    beaconProtocol->send_data.mode_data.round.receiver2.nbr_rd = hydro->uwave.puwv3.counterID2;
    qDebug() << "hydro->uwave.puwv3.counterID2: " << hydro->uwave.puwv3.counterID2;
    beaconProtocol->send_data.mode_data.round.receiver2.nbr_td = hydro->uwave.counterACK2;
    qDebug() << "hydro->uwave.counterACK2: " << hydro->uwave.counterACK2;
    beaconProtocol->send_data.mode_data.round.receiver2.tFlight = hydro->uwave.puwv3.distanceID2;
    qDebug() << "hydro->uwave.puwv3.distanceID2: " << hydro->uwave.puwv3.distanceID2;
    beaconProtocol->send_data.mode_data.round.receiver3.nbr_rd = hydro->uwave.puwv3.counterID3;
    qDebug() << "hydro->uwave.puwv3.counterID3: " << hydro->uwave.puwv3.counterID3;
    beaconProtocol->send_data.mode_data.round.receiver3.nbr_td = hydro->uwave.counterACK3;
    qDebug() << "hydro->uwave.counterACK3: " << hydro->uwave.counterACK3;
    beaconProtocol->send_data.mode_data.round.receiver3.tFlight = hydro->uwave.puwv3.distanceID3;
    qDebug() << "hydro->uwave.puwv3.distanceID3: " << hydro->uwave.puwv3.distanceID3;
    beaconProtocol->send_data.mode_data.round.receiver4.nbr_rd = hydro->uwave.puwv3.counterID4;
    qDebug() << "hydro->uwave.puwv3.counterID4: " << hydro->uwave.puwv3.counterID4;
    beaconProtocol->send_data.mode_data.round.receiver4.nbr_td = hydro->uwave.counterACK4;
    qDebug() << "hydro->uwave.counterACK4: " << hydro->uwave.counterACK4;
    beaconProtocol->send_data.mode_data.round.receiver4.tFlight = hydro->uwave.puwv3.distanceID4;
    qDebug() << "hydro->uwave.puwv3.distanceID4: " << hydro->uwave.puwv3.distanceID4;

    qDebug() << &(beaconProtocol->rec_data.mode_select);
    qDebug() << "distance_real" << beaconProtocol->rec_data.distance_real;

    if ((hydro->uwave.puwv3.temp < -5) or (hydro->uwave.puwv3.temp >40))
    {
        hydro->uwave.warning ++;
        beaconProtocol->send_data.WARNING = hydro->uwave.warning;
    }
    if ((hydro->uwave.puwv3.tempID1 < -5) or (hydro->uwave.puwv3.tempID1 >40))
    {
        hydro->uwave.warning ++;
        beaconProtocol->send_data.WARNING = hydro->uwave.warning;
    }
    if ((hydro->uwave.puwv3.tempID2 < -5) or (hydro->uwave.puwv3.tempID2 >40))
    {
        hydro->uwave.warning ++;
        beaconProtocol->send_data.WARNING = hydro->uwave.warning;
    }
    if ((hydro->uwave.puwv3.tempID3 < -5) or (hydro->uwave.puwv3.tempID3 >40))
    {
        hydro->uwave.warning ++;
        beaconProtocol->send_data.WARNING = hydro->uwave.warning;
    }
    if ((hydro->uwave.puwv3.tempID4 < -5) or (hydro->uwave.puwv3.tempID4 >40))
    {
        hydro->uwave.warning ++;
        beaconProtocol->send_data.WARNING = hydro->uwave.warning;
    }
    qDebug() << "WARNING: " << beaconProtocol->send_data.WARNING ;

//    emit updateUpdate(uwave);
}

void ControlBeacon::updateTime(float time)
{
    timeQuest = time*1000;
//    qDebug() << "timeQuest: " << timeQuest;
}


