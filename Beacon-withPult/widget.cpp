//#include "widget.h"
//#include "ui_widget.h"
//#include <QObject>

//Widget::Widget(QWidget *parent)
//    : QWidget(parent)
//    , ui(new Ui::Widget)
//{
//    ui->setupUi(this);
//    connect(ui->btnDirect, &QPushButton::clicked, this, &Widget::updateModeDirect);
//    connect(ui->btnRoundR, &QPushButton::clicked, this, &Widget::updateModeRound);
//    connect(ui->btnStop, &QPushButton::clicked, this, &Widget::stop);
////    connect(ui->doubleSpinBoxTime, &QDoubleSpinBox::valueChanged(double), this, &Widget::updateTime);
////    connect(ui->doubleSpinBoxTime, SIGNAL(valueChanged(double)), this, SLOT(updateTime));
//    connect(ui->doubleSpinBoxTime, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Widget::updateTime);
////    connect(ui->btnDirect, &QPushButton::clicked, cb, &ControlBeacon::startRoundR);
////    connect(ui->btnRoundR, &QPushButton::clicked, cb, &ControlBeacon::startDirectConnection);
////    connect(ui->btnStop, &QPushButton::clicked, cb, &ControlBeacon::finishExchange);
//}




//Widget::~Widget()
//{
//    delete ui;
//}

//void Widget::update(uWave uwave)
//{
//    ui->goodUnknown->setNum(uwave.puwv3.counter);
//    ui->notGoodUnknown->setNum(uwave.puwv4.counter);
//    ui->goodAll->setNum(uwave.puwv3.counterAll);
//    ui->notGoodAll->setNum(uwave.puwv4.counterAll);
//    if (ch.txCh == 1) ui->goodDirect->setNum(uwave.puwv3.counterID1);
//    if (ch.txCh == 2) ui->goodDirect->setNum(uwave.puwv3.counterID2);
//    if (ch.txCh == 3) ui->goodDirect->setNum(uwave.puwv3.counterID3);
//    if (ch.txCh == 4) ui->goodDirect->setNum(uwave.puwv3.counterID4);
//    if (ch.txCh == 1) ui->bedDirect->setNum(uwave.puwv4.counterID1);
//    if (ch.txCh == 2) ui->bedDirect->setNum(uwave.puwv4.counterID2);
//    if (ch.txCh == 3) ui->bedDirect->setNum(uwave.puwv4.counterID3);
//    if (ch.txCh == 4) ui->bedDirect->setNum(uwave.puwv4.counterID4);
//    ui->bedRoundID1->setNum(uwave.puwv4.counterID1);
//    ui->bedRoundID2->setNum(uwave.puwv4.counterID2);
//    ui->bedRoundID3->setNum(uwave.puwv4.counterID3);
//    ui->bedRoundID4->setNum(uwave.puwv4.counterID4);
//    ui->goodRoundID1->setNum(uwave.puwv3.counterID1);
//    ui->goodRoundID2->setNum(uwave.puwv3.counterID2);
//    ui->goodRoundID3->setNum(uwave.puwv3.counterID3);
//    ui->goodRoundID4->setNum(uwave.puwv3.counterID4);
//    if (ch.txCh == 1) ui->distanceDirect->setNum(uwave.puwv3.distanceID1);
//    if (ch.txCh == 2) ui->distanceDirect->setNum(uwave.puwv3.distanceID2);
//    if (ch.txCh == 3) ui->distanceDirect->setNum(uwave.puwv3.distanceID3);
//    if (ch.txCh == 4) ui->distanceDirect->setNum(uwave.puwv3.distanceID4);
//    ui->distanceRoundID1->setNum(uwave.puwv3.distanceID1);
//    ui->distanceRoundID2->setNum(uwave.puwv3.distanceID2);
//    ui->distanceRoundID3->setNum(uwave.puwv3.distanceID3);
//    ui->distanceRoundID4->setNum(uwave.puwv3.distanceID4);
//    ui->labelcounterACK->setNum(uwave.counterACK);
//    ui->counterACK1->setNum(uwave.counterACK1);
//    ui->counterACK2->setNum(uwave.counterACK2);
//}

//void Widget::updateModeDirect()
//{
//    ch.txCh = ui->spinBoxexamineChannel->value();
//    ch.rxCh = ui->spinBoxwaitChennel->value();
//    emit startDirect(ch);
//}

//void Widget::updateModeRound()
//{
//    chRW.txCh1 = ui->spinBoxTxCh1->value();
//    chRW.txCh2 = ui->spinBoxTxCh2->value();
//    chRW.txCh3 = ui->spinBoxTxCh3->value();
//    chRW.txCh4 = ui->spinBoxTxCh4->value();
//    chRW.rxCh = ui->spinBoxRxCh->value();
//    chRW.Number = ui->spinBoxNumber->value();
//    emit startRoundR(chRW);
//}

//void Widget::updateTime(double time)
//{
//    emit signalTimeUpdate(time);
//}



