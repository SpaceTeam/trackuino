#ifndef Flight_h
#define Flight_h

#include "Arduino.h"
#include "Sensors.h"
#include "Bar.h"
#include "Acc.h"
#include "HighG.h"
#include "Gyro.h"

#define TRIGGER_LEVEL_ACC 5 //in g
#define TRIGGER_ON_TIME 100 //in ms
#define FLIGHT_TIME 5       //in mins

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

