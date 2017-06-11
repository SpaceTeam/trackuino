#ifndef Sensors_h
#define Sensors_h

#include "Arduino.h"
#include "SPIX0.h"
#include "Bar.h"
#include "Acc.h"
#include "HighG.h"
#include "Gyro.h"

#define PERIOD_TEMP_MEAS 100

enum sensorsState {
  waitBar,
  waitAcc,
  waitHG,
  waitGyro,
  waitFlash
};

class Sensors
{
  public:
  Acc acc;
  HighG hg;
  Bar bar;
  Gyro gyro;

  sensorsState state;

  int32_t TEMP, P;

  void Init();
  void Handle();
  void HandleFastMode();
  void saveSensorsPoint();
};

#endif

