#include "ms5837.h"

const uint8_t MS5837_ADDR = 0x76;
const uint8_t MS5837_RESET = 0x1E;
const uint8_t MS5837_ADC_READ = 0x00;
const uint8_t MS5837_PROM_READ = 0xA0;
const uint8_t MS5837_CONVERT_D1_8192 = 0x4A;
const uint8_t MS5837_CONVERT_D2_8192 = 0x5A;

const float MS5837::Pa = 100.0f;
const float MS5837::bar = 0.001f;
const float MS5837::mbar = 1.0f;

const uint8_t MS5837::MS5837_30BA = 0;
const uint8_t MS5837::MS5837_02BA = 1;
const uint8_t MS5837::MS5837_UNRECOGNISED = 255;


const uint8_t MS5837_02BA01 = 0x00; // Sensor version: From MS5837_02BA datasheet Version PROM Word 0
const uint8_t MS5837_02BA21 = 0x15; // Sensor version: From MS5837_02BA datasheet Version PROM Word 0
const uint8_t MS5837_30BA26 = 0x1A; // Sensor version: From MS5837_30BA datasheet Version PROM Word 0

extern double X[2000][2];

MS5837::MS5837(QObject *parent) : timerPS(this)
{
    connect(&timerPS, &QTimer::timeout, this, &MS5837::timeoutSlot);
}

void MS5837::startPS()
{
    if (!init())
    {
        qDebug()<<"Датчик давления не инициализирован";
    }
    else
    {
        qDebug()<<"Датчик давления инициализирован";
        setModel(MS5837::MS5837_02BA);
        setFluidDensity(997);
//            emit started();
            timerPS.start(500);
            qDebug() << "timer.start(50)";

            return;
    }
}
//инициализация
bool MS5837::init()
{
    C[0]=0;
    C[1]=0;
    C[2]=0;
    C[3]=0;
    C[4]=0;
    C[5]=0;
    C[6]=0;
    C[7]=0;
// проверяется командой i2cdetect -y 1
    fd = wiringPiI2CSetup( MS5837_ADDR);
    qDebug() << "fd: " << fd;
   if (fd == -1)
   {
       qDebug() <<"потеряна связь с датчиком давления";
       return -1;
   }
       qDebug()<<"связь с датчиком давления есть)";

   wiringPiI2CWrite(fd, MS5837_RESET);
//   delay(100);

   for (uint8_t i=0 ; i<7; i++)
   {
       i2c_smbus_read_i2c_block_data(fd, MS5837_PROM_READ+i*2, 2, (__u8*)(C+i));
//        delay(5);

        uint8_t temp = *(__u8*)(C+i);
        *(__u8*)(C+i) = *((__u8*)(C+i)+1);
        *((__u8*)(C+i)+1) = temp;
   }

   uint8_t crcRead= C[0]>>12;
   uint8_t crcCalculated=crc4(C);

   qDebug()<<"crcRead      : " <<crcRead;
   qDebug()<<"crcCalculated: " <<crcCalculated;


   if ((crcCalculated!=crcRead)|(crcCalculated==0)|(crcRead==0))
   {
       return false;
   }

    uint8_t version = (C[0] >> 5) & 0x7F; // Извлеките версию датчика из PROM Word 0
    qDebug() <<"version: " << version;
  // Установите _model в соответствии с версией датчика
    //if (version == MS5837_02BA01)
      //  {
        //    _model = MS5837_02BA;
        //}
        //else if (version == MS5837_02BA21)
        //{
        //    _model = MS5837_02BA;
       // }
        if (version == MS5837_30BA26)
        {
            _model = MS5837_30BA;
        }
        else
        {
            _model = MS5837_UNRECOGNISED;
        }

  return true;
}

void MS5837::setModel(uint8_t model) {
    _model = model;
}




void MS5837::setFluidDensity(float density)
{
      fluidDensity = density;
}

void MS5837::read()
{
    wiringPiI2CWrite(fd,MS5837_CONVERT_D1_8192);
    delay(20);// Максимальное время преобразования для каждой таблицы данных

    D1_pres=0;
//    uint8_t buffer[3];
//    buffer[0]=buffer[1]=buffer[2]=0;
    i2c_smbus_read_i2c_block_data(fd, MS5837_ADC_READ, 3, (__u8*)(&D1_pres));
//     i2c_smbus_read_i2c_block_data(fd, MS5837_ADC_READ, 3, buffer);
    delay(20);
//    qDebug() <<"buffer: " << buffer[0] <<  buffer[1] <<  buffer[2];
    uint8_t temp = *((__u8*)(&D1_pres));
    *((__u8*)(&D1_pres)) = *((__u8*)(&D1_pres)+2);
    *((__u8*)(&D1_pres)+2) = temp;
    delay(20);

    wiringPiI2CWrite(fd,MS5837_CONVERT_D2_8192);
    delay(20);// Максимальное время преобразования для каждой таблицы данных

    D2_temp=0;

    i2c_smbus_read_i2c_block_data(fd, MS5837_ADC_READ, 3, (__u8*)(&D2_temp));
    delay(20);

    uint8_t temp1 = *((__u8*)(&D2_temp));
    *((__u8*)(&D2_temp)) = *((__u8*)(&D2_temp)+2);
    *((__u8*)(&D2_temp)+2) = temp1;

    calculate();
}

//вычисления
void MS5837::calculate() {
    // Given C1-C6 and D1, D2, calculated TEMP and P
    // Do conversion first and then second order temp compensation

    int32_t dT = 0;
    int64_t SENS = 0;
    int64_t OFF = 0;
    int32_t SENSi = 0;
    int32_t OFFi = 0;
    int32_t Ti = 0;
    int64_t OFF2 = 0;
    int64_t SENS2 = 0;

    // Terms called
    dT = D2_temp - static_cast<uint32_t>(C[5]) * 256l;
    //if (_model == MS5837_02BA) {
   //     SENS = static_cast<int64_t>(C[1]) * 65536l + (static_cast<int64_t>(C[3]) * dT) / 128l;
    //    OFF = static_cast<int64_t>(C[2]) * 131072l + (static_cast<int64_t>(C[4]) * dT) / 64l;
    //    P = (D1_pres * SENS / (2097152l) - OFF) / (32768l);
   // } else {
        SENS = static_cast<int64_t>(C[1]) * 32768l + (static_cast<int64_t>(C[3]) * dT) / 256l;
        OFF = static_cast<int64_t>(C[2]) * 65536l + (static_cast<int64_t>(C[4]) * dT) / 128l;
        P = (D1_pres * SENS / (2097152l) - OFF) / (8192l);
   // }

    // Temp conversion
    TEMP = 2000l + static_cast<int64_t>(dT) * C[6] / 8388608LL;

    // Second order compensation
    //if (_model == MS5837_02BA) {
      //  if ((TEMP / 100) < 20) { // Low temp
        //    Ti = (11 * static_cast<int64_t>(dT) * static_cast<int64_t>(dT)) / 34359738368LL;
          //  OFFi = (31 * (TEMP - 2000) * (TEMP - 2000)) / 8;
          //  SENSi = (63 * (TEMP - 2000) * (TEMP - 2000)) / 32;
       // }
   // } else {
        if ((TEMP / 100) < 20) { // Low temp
            Ti = (3 * static_cast<int64_t>(dT) * static_cast<int64_t>(dT)) / 8589934592LL;
            OFFi = (3 * (TEMP - 2000) * (TEMP - 2000)) / 2;
            SENSi = (5 * (TEMP - 2000) * (TEMP - 2000)) / 8;
            if ((TEMP / 100) < -15) { // Very low temp
                OFFi = OFFi + 7 * (TEMP + 1500l) * (TEMP + 1500l);
                SENSi = SENSi + 4 * (TEMP + 1500l) * (TEMP + 1500l);
            }
        } else if ((TEMP / 100) >= 20) { // High temp
            Ti = 2 * (dT * dT) / 137438953472LL;
            OFFi = (1 * (TEMP - 2000) * (TEMP - 2000)) / 16;
            SENSi = 0;
        }
   // }

    OFF2 = OFF - OFFi; // Calculate pressure and temp second order
    SENS2 = SENS - SENSi;

    TEMP = (TEMP - Ti);
//    qDebug() << "_model: " << _model;
   // if (_model == MS5837_02BA) {
       // P = (((D1_pres * SENS2) / 2097152l - OFF2) / 32768l);
   // } else {
        P = (((D1_pres * SENS2) / 2097152l - OFF2) / 8192l);
    //}
}

float MS5837::pressure(float conversion)
{
        return P*conversion/10.0f;
}

float MS5837::temperature()
{
    return TEMP/100.0f;
}

float MS5837::depth()
{
    return (pressure(MS5837::Pa)-102400)/(fluidDensity*9.80665);
}

float MS5837::altitude()
{
    return (1-pow((pressure()/1013.25),.190284))*145366.45*.3048;
}

//Контр сумма
uint8_t MS5837::crc4(uint16_t n_prom[])
{
    uint16_t n_rem = 0;

    n_prom[0] = ((n_prom[0]) & 0x0FFF);
    n_prom[7] = 0;

    for ( uint8_t i = 0 ; i < 16; i++ ) {
        if ( i%2 == 1 ) {
            n_rem ^= (uint16_t)((n_prom[i>>1]) & 0x00FF);
        } else {
            n_rem ^= (uint16_t)(n_prom[i>>1] >> 8);
        }
        for ( uint8_t n_bit = 8 ; n_bit > 0 ; n_bit-- ) {
            if ( n_rem & 0x8000 ) {
                n_rem = (n_rem << 1) ^ 0x3000;
            } else {
                n_rem = (n_rem << 1);
            }
        }
    }

    n_rem = ((n_rem >> 12) & 0x000F);

    return n_rem ^ 0x00;
}

void MS5837::timeoutSlot()
{
    read();
    qDebug() << "timeoutSlot";
//    qDebug() << "altitude   : " << altitude();
//    qDebug() << "pressure   : " << pressure();
//    qDebug() << "temperature: " << temperature();
    qDebug() << "depth      : " << depth();
    //X[609][0] = depth()+K[899];
}


void MS5837::getDepth() { // Слот для обработки запроса
//        read(); // Обновить данные
        float currentDepth = depth();
        //X[609][0] = depth()+K[899];
        emit depthReceived(currentDepth);
    }
