/* trackuino copyright (C) 2010  EA5HAV Javi

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

// Mpide 22 fails to compile Arduino code because it stupidly defines ARDUINO
// as an empty macro (hence the +0 hack). UNO32 builds are fine. Just use the
// real Arduino IDE for Arduino builds. Optionally complain to the Mpide
// authors to fix the broken macro.
#if (ARDUINO + 0) == 0
#error "Oops! We need the real Arduino IDE (version 22 or 23) for Arduino builds."
#error "See trackuino.pde for details on this"

// Refuse to compile on arduino version 21 or lower. 22 includes an
// optimization of the USART code that is critical for real-time operation
// of the AVR code.
#elif (ARDUINO + 0) < 22
#error "Oops! We need Arduino 22 or 23"
#error "See trackuino.pde for details on this"

#endif


// Trackuino custom libs
#include "config.h"
#include "afsk_avr.h"
#include "aprs.h"
#include "gps.h"
#include "power.h"
#include "radio_hx1.h"

// Arduino/AVR libs
#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif

#include "SPIX0.h"
#include "SPIX1.h"
#include "Flash.h"
#include "Sensors.h"
#include "GPSX.h"

// Module constants
static const uint32_t VALID_POS_TIMEOUT = 2000;  // ms
static uint32_t gps_timeout;

// Module variables
static int32_t next_aprs = 0;

Sensors sensors;
GPSX gpsx;

void setup()
{
  //blink led
  DDRE |= 1 << 2;
  for (int i = 0; i < 5; i++)
  {
    PORTE |= 1 << 2;
    delay(200);
    PORTE &= ~(1 << 2);
    delay(200);
  }

  //5v on
  PORTC |= 1 << 3;
  DDRC |= (1 << 3);

  spi0.Init();
  spi1.Init();
  Serial.begin(GPS_BAUDRATE);

  bool flashErase = flash.Init();
  afsk_setup();
  gps_setup();
  static RadioHx1 radio;
  radio.setup();

  gpsx.Init();
  sensors.Init();

  if (flashErase)
  {
    while (flash.RDSR1() & 1); //wait until flash is erased
  }

  gps_reset_parser();
  gps_timeout = millis();
  while (gps_fixq == 0) get_pos(); //wait for gps fix

  next_aprs = millis();
}

void get_pos()
{
  // Get a valid position from the GPS
  int valid_pos = 0;

  if (millis() - gps_timeout > VALID_POS_TIMEOUT)
  {
    gps_reset_parser();
    gps_timeout = millis();
  }

  while (Serial.available() && !valid_pos)
  {
    valid_pos = gps_decode(Serial.read());
  }

  if (valid_pos) {
    gpsx.saveGPSPoint();
    gps_reset_parser();
  }
}

void loop()
{
  get_pos();

  sensors.Handle();

  // Time for another APRS frame
  if ((int32_t) (millis() - next_aprs) >= 0) {
    aprs_send();
    next_aprs += APRS_PERIOD * 1000L;
    while (afsk_flush()) {
      sensors.HandleFastMode();
    }

#ifdef DEBUG_MODEM
    // Show modem ISR stats from the previous transmission
    afsk_debug();
#endif
  }
}
