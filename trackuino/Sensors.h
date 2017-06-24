#ifndef Sensors_h
#define Sensors_h

#include "Arduino.h"
#include "SPIX0.h"
#include "Bar.h"
#include "Acc.h"
#include "HighG.h"
#include "Gyro.h"
#include "Flight.h"

#define PERIOD_TEMP_MEAS 100
#define SAVE_PERIOD_IDLE 10

enum fastModeState {
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

  fastModeState state;
  uint32_t timestamp_last_point_saved;

  int32_t TEMP, P;

  void Init();
  void Handle();
  void HandleFastMode();
  void saveSensorsPoint();
};

extern Sensors sensors;

#endif

