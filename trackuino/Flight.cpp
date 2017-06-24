#include "Flight.h"

Flight flight;

void Flight::Handle()
{
  switch (state)
  {
    case flightStateIdle:
      if (TriggerOn())
      {
        trigger_timestamp = millis();
        state = flightStateTrigger;
      }
      break;

    case flightStateTrigger:
      if (TriggerOn())
      {
        if (millis() - trigger_timestamp > TRIGGER_ON_TIME)
        {
          state = flightStateFlight;
          flight_timestamp = millis();
        }
      }
      else
      {
        state = flightStateIdle;
      }
      break;

    case flightStateFlight:
      if (millis() - flight_timestamp > (uint32_t)FLIGHT_TIME * 60000)
      {
        state = flightStateIdle;
      }
      break;
  }
}

bool Flight::TriggerOn()
{
    if (sensors.hg.accx > TRIGGER_LEVEL_ACC / HIGH_G_LSB || sensors.hg.accy > TRIGGER_LEVEL_ACC / HIGH_G_LSB || sensors.hg.accz > TRIGGER_LEVEL_ACC / HIGH_G_LSB)
    {
      return true;
    }

  return false;
}

