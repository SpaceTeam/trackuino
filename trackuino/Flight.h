#ifndef Flight_h
#define Flight_h

#include "Arduino.h"
#include "Sensors.h"
#include "Bar.h"
#include "Acc.h"
#include "HighG.h"
#include "Gyro.h"
#include "GPSX.h"
#include "config.h"
#include "configX.h"

enum flightState {
  flightStateIdle = 0,
  flightStateTrigger = 1,
  flightStateFlight = 2,
  flightStateFindPos = 3,
  flightStatePing = 4
};

class Flight
{
  public:
  uint32_t timestamp;
  uint32_t actual_aprs_period = (uint32_t)APRS_PERIOD * 1000;

  flightState state = flightStateIdle;

  void Handle();
  bool TriggerOn();
};

extern Flight flight;

#endif

