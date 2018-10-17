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
      if (millis() - timestamp > (uint32_t)FLIGHT_TIME * 1000)
      {
        state = flightStateSend;
        timestamp = millis();
      }
      break;

    case flightStateSend:
      if (millis() - timestamp > (uint32_t)SEND_TIME * 1000)
      {
        state = flightStatePause;
        timestamp = millis();

        gpsx.CFG_RATE(5000, 1, 1);
        sensors.acc.Standby();
        sensors.hg.Standby();
        sensors.gyro.PowerDown();
      }
      break;

    case flightStatePause:
      if (millis() - timestamp > (uint32_t)PAUSE_TIME * 1000)
      {
        state = flightStateLanded;

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

