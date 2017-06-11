#include "Sensors.h"

void Sensors::Init()
{
  bar.Init();
  acc.Init();
  hg.Init();
  gyro.Init();

  bar.PerformPressMeas();

  state = waitBar;
}

void Sensors::Handle()
{
  while (!bar.ReadD1()) {}
  if (millis() - bar.timeLastTempMeas > PERIOD_TEMP_MEAS)
  {
    bar.PerformTempMeasAndReadD2();
    bar.timeLastTempMeas = millis();
  }
  bar.PerformPressMeas();

  while(!acc.ReadXYZ()) {}

  while(!hg.ReadXYZ()) {}

  while(!gyro.ReadXYZ()) {}

  while(!((flash.RDSR1() & 1) == 0)) {}

  saveSensorsPoint();
}

void Sensors::HandleFastMode()
{
  switch (state)
  {
    case waitBar:
      if (!bar.ReadD1()) return;

      bar.PerformPressMeas();
      
      state = waitAcc;
      break;

    case waitAcc:
      if (!acc.ReadXYZ()) return;

      state = waitHG;
      break;

    case waitHG:
      if (!hg.ReadXYZ()) return;

      state = waitGyro;
      break;

    case waitGyro:
      if (!gyro.ReadXYZ()) return;

      state = waitFlash;
      break;

    case waitFlash:
      if (flash.RDSR1() & 1 == 0) return;

      saveSensorsPoint();

      state = waitBar;
      break;
  }
}

void Sensors::saveSensorsPoint()
{
  byte sensorPoint[POINT_LEN_SENSORS];
  uint32_t timeStamp = micros();

  sensorPoint[0] = POINT_INDICATOR_SENSORS;

  sensorPoint[1] = timeStamp >> 24;
  sensorPoint[2] = timeStamp >> 16;
  sensorPoint[3] = timeStamp >> 8;
  sensorPoint[4] = timeStamp;

  sensorPoint[POINT_POS_BAR] = bar.D1 >> 16;
  sensorPoint[POINT_POS_BAR + 1] = bar.D1 >> 8;
  sensorPoint[POINT_POS_BAR + 2] = bar.D1;
  sensorPoint[POINT_POS_BAR + 3] = bar.D2 >> 16;
  sensorPoint[POINT_POS_BAR + 4] = bar.D2 >> 8;
  sensorPoint[POINT_POS_BAR + 5] = bar.D2;

  sensorPoint[POINT_POS_ACC] = acc.accx >> 8;
  sensorPoint[POINT_POS_ACC + 1] = acc.accx;
  sensorPoint[POINT_POS_ACC + 2] = acc.accy >> 8;
  sensorPoint[POINT_POS_ACC + 3] = acc.accy;
  sensorPoint[POINT_POS_ACC + 4] = acc.accz >> 8;
  sensorPoint[POINT_POS_ACC + 5] = acc.accz;

  sensorPoint[POINT_POS_HG] = hg.accx >> 8;
  sensorPoint[POINT_POS_HG + 1] = hg.accx;
  sensorPoint[POINT_POS_HG + 2] = hg.accy >> 8;
  sensorPoint[POINT_POS_HG + 3] = hg.accy;
  sensorPoint[POINT_POS_HG + 4] = hg.accz >> 8;
  sensorPoint[POINT_POS_HG + 5] = hg.accz;

  sensorPoint[POINT_POS_GYRO] = gyro.spinx >> 8;
  sensorPoint[POINT_POS_GYRO + 1] = gyro.spinx;
  sensorPoint[POINT_POS_GYRO + 2] = gyro.spiny >> 8;
  sensorPoint[POINT_POS_GYRO + 3] = gyro.spiny;
  sensorPoint[POINT_POS_GYRO + 4] = gyro.spinz >> 8;
  sensorPoint[POINT_POS_GYRO + 5] = gyro.spinz;

  flash.Write(sensorPoint, POINT_LEN_SENSORS);
}

