#include <WiFiUdp.h>
#include <ArduinoJson.h>

#include "_config.h"

#include "system.h"
#include "sensors.h"
#include "communication.h"

void send_udp_data_run();
Thread send_udp_data(send_udp_data_run, 500);

WiFiUDP Udp;

const size_t bufferSize = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(9);

DynamicJsonDocument doc(bufferSize);

char output[bufferSize];

void Communication::init(){
  LOG("Communication::init");ENDL;
  
  Udp.begin(3334);  
  CPU.add(&send_udp_data);
}

JsonObject weather = doc.createNestedObject("weather");

void send_udp_data_run(){
  noInterrupts();
  weather["altitude"] = Sensors::altitude;
  weather["pressure"] = Sensors::pressure;
  weather["temperature"] = (Sensors::DHT_temp_avg + Sensors::BMP_temp)/2.0;
  weather["humidity"] = Sensors::DHT_hum_avg;
  weather["heading"] = Sensors::heading;
  weather["windDir"] = Sensors::windDirection;
  weather["windSpeed"] = Sensors::windSpeed;
  weather["low_battery"] = !System::battery_state;

  serializeJson(weather, output);

  Udp.beginPacket("0.0.0.0", 3334);
    Udp.write(output);
  Udp.endPacket();
  interrupts();

}