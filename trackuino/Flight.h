#ifndef Flight_h
#define Flight_h

#include "Arduino.h"
#include "Sensors.h"
#include "Bar.h"
#include "Acc.h"
#include "HighG.h"
#include "Gyro.h"
#include "configX.h"

enum flightState {
  flightStateIdle,
  flightStateTrigger,
  flightStateFlight
};

class Flight
{
  public:
  uint32_t trigger_timestamp, flight_timestamp;

  flightState state = flightStateIdle;

  void Handle();
  bool TriggerOn();
};

extern Flight flight;

#endif

