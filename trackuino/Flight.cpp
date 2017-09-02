#include "Flight.h"

Flight flight;

void Flight::Handle()
{
  switch (state)
  {
    case flightStateIdle:
      if (TriggerOn())
      {
        timestamp = millis();
        state = flightStateTrigger;
      }
      break;

    case flightStateTrigger:
      if (TriggerOn())
      {
        if (millis() - timestamp > TRIGGER_ON_TIME)
        {
          state = flightStateFlight;
          timestamp = millis();
        }
      }
      else
      {
        state = flightStateIdle;
      }
      break;

    case flightStateFlight:
      if (millis() - timestamp > (uint32_t)FLIGHT_TIME * 60000)
      {
        state = flightStateFindPos;
        timestamp = millis();

        gpsx.CFG_RATE(5000, 1, 1);
        sensors.acc.Standby();
        sensors.hg.Standby();
        sensors.gyro.PowerDown();
      }
      break;

    case flightStateFindPos:
      if (millis() - timestamp > (uint32_t)FIND_POS_TIME * 60000)
      {
        state = flightStatePing;

        actual_aprs_period = (uint32_t)APRS_PERIOD_LANDED * 1000;
        gpsx.RXM_PMREQ(0, 2); //set gps module to backup mode
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

