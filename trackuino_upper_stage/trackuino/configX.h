#ifndef __CONFIGX_H__
#define __CONFIGX_H__

#define PERIOD_TEMP_MEAS 100
#define SAVE_PERIOD_IDLE 10

#define POINT_INDICATOR_SENSORS 0xd2
#define POINT_INDICATOR_GPS 0xb9
#define POINT_INDICATOR_RESET 0x13

#define POINT_LEN_SENSORS 33
#define POINT_LEN_GPS 41

#define POINT_POS_BAR  9
#define POINT_POS_ACC  15
#define POINT_POS_HG   21
#define POINT_POS_GYRO 27

#define POINT_POS_TIME 5
#define POINT_POS_LAT  12
#define POINT_POS_LON  23
#define POINT_POS_ALT  35
#define POINT_POS_SATS 39
#define POINT_POS_FIXQ 40

#define PAGE_SIZE 256
#define MEM_SIZE 32768000

#define TRIGGER_LEVEL_ACC 5   //in g
#define TRIGGER_ON_TIME 100   //in ms
#define FLIGHT_TIME 300         //in s
#define SEND_TIME 600      //in s
#define PAUSE_TIME 600      //in s
#define APRS_PERIOD_LANDED 30  //in s

#define COM_TIMEOUT 10000 //wait 10 secs until pc responds

#define ACC_LSB 4e-3
#define HIGH_G_LSB 49e-3
#define GYRO_LSB 1.0

#define BAR_TIMEOUT 2000 //in us
#define BAR_PERFORM_TIME 800 //in us

#endif


