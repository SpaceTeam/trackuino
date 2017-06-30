#include "GPSX.h"

void GPSX::Init()
{
  setFlightMode();
  CFG_RATE(100, 1, 1);
}

// Send a byte array of UBX protocol to the GPS
void GPSX::sendUBX(uint8_t *MSG, uint8_t len)
{
  for (int i = 0; i < len; i++) {
    Serial.write(MSG[i]);
  }
  Serial.println();
}


// Calculate expected UBX ACK packet and parse UBX response from GPS
boolean GPSX::getUBX_ACK(uint8_t *MSG)
{
  uint8_t b;
  uint8_t ackByteID = 0;
  uint8_t ackPacket[10];
  unsigned long startTime = millis();

  // Construct the expected ACK packet
  ackPacket[0] = 0xB5;  // header
  ackPacket[1] = 0x62;  // header
  ackPacket[2] = 0x05;  // class
  ackPacket[3] = 0x01;  // id
  ackPacket[4] = 0x02;  // length
  ackPacket[5] = 0x00;
  ackPacket[6] = MSG[2];  // ACK class
  ackPacket[7] = MSG[3];  // ACK id
  ackPacket[8] = 0;   // CK_A
  ackPacket[9] = 0;   // CK_B

  // Calculate the checksums
  for (uint8_t i = 2; i < 8; i++) {
    ackPacket[8] = ackPacket[8] + ackPacket[i];
    ackPacket[9] = ackPacket[9] + ackPacket[8];
  }

  while (1) {

    // Test for success
    if (ackByteID > 9) {
      // All packets in order!
      return true;
    }

    // Timeout if no valid response in 3 seconds
    if (millis() - startTime > 3000) {
      Serial.println(" (FAILED!)");
      return false;
    }

    // Make sure data is available to read
    if (Serial.available()) {
      b = Serial.read();

      // Check that bytes arrive in sequence as per expected ACK packet
      if (b == ackPacket[ackByteID]) {
        ackByteID++;
      }
      else {
        ackByteID = 0;  // Reset and look again, invalid order
      }

    }
  }
}

union floatConvert
{
  float fl;
  byte bytes[4];
};

void GPSX::saveGPSPoint()
{
  floatConvert conv;
  byte point[POINT_LEN_GPS];
  uint32_t timeStamp = micros();

  point[0] = POINT_INDICATOR_GPS;

  point[1] = timeStamp >> 24;
  point[2] = timeStamp >> 16;
  point[3] = timeStamp >> 8;
  point[4] = timeStamp;

  memcpy(point + POINT_POS_TIME, gps_time, 6);

  memcpy(point + POINT_POS_LAT, gps_save_lat, 11);

  memcpy(point + POINT_POS_LON, gps_save_lon, 12);

  conv.fl = gps_altitude;
  memcpy(point + POINT_POS_ALT, conv.bytes, 4);

  point[POINT_POS_SATS] = gps_satellites;

  point[POINT_POS_FIXQ] = gps_fixq;

  flash.Write(point, POINT_LEN_GPS);
}

void GPSX::buildUBXpacket(byte CLASS, byte ID, uint16_t LENGTH, byte* Payload, byte* Packet)
{
  //calculate checksum
  byte CK_A = 0;
  byte CK_B = 0;
  Packet[0] = 0xb5;
  Packet[1] = 0x62;
  Packet[2] = CLASS;
  Packet[3] = ID;
  Packet[4] = LENGTH;
  Packet[5] = LENGTH >> 8;
  memcpy(Packet + 6, Payload, LENGTH);
  Packet[LENGTH + 6] = 0;
  Packet[LENGTH + 7] = 0;

  // Calculate the checksums
  for (uint8_t i = 2; i < LENGTH + 6; i++) {
    CK_A = CK_A + Packet[i];
    CK_B = CK_B + CK_A;
  }

  Packet[LENGTH + 6] = CK_A;
  Packet[LENGTH + 7] = CK_B;
}

void GPSX::sendUBX_and_getUBX_ACK(byte* Packet, int len)
{
  bool gps_set_sucess = false;
  while (!gps_set_sucess)
  {
    sendUBX(Packet, len);
    gps_set_sucess = getUBX_ACK(Packet);
  }
}

void GPSX::setFlightMode()
{
  //  THIS COMMAND SETS FLIGHT MODE AND CONFIRMS IT
  uint8_t setNav[] = {
    0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00,
    0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0xDC
  };

  sendUBX_and_getUBX_ACK(setNav, sizeof(setNav) / sizeof(uint8_t));
}

void GPSX::CFG_RATE(uint16_t measRate, uint16_t navRate, uint16_t timeRef)
{
  uint8_t packet[14];
  uint8_t payload[] =  {measRate, measRate >> 8, navRate, navRate >> 8, timeRef, timeRef >> 8};

  buildUBXpacket(0x06, 0x08, 6, payload, packet);

  sendUBX_and_getUBX_ACK(packet, 14);
}
