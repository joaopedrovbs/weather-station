#include "Thread.h"
#include "ThreadController.h"

#ifndef SENSORS_H
#define SENSORS_H

class Sensors{
public:
  static void init();
  static float DHT_temp_avg; 
  static float DHT_hum_avg;
  static float heading;
  static uint8_t windDirection;
  static float windSpeed;
  static float pressure;
  static float BMP_temp;
  static float altitude;

};
#endif