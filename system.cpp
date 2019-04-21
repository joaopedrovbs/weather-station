#include <Thread.h>
#include <ESP8266WiFi.h>

#include "_config.h"

#include "system.h"

// Main Thread Controller
ThreadController CPU;

bool System::battery_state;

// Battery Check Thread Initialization
void battery_checker_run();
Thread thread_battery_checker(battery_checker_run, 1000);

void System::init(){

  // Setup Inputs
  pinMode(BAT_SENSOR, INPUT);

  // Setup Outputs
  pinMode(LOG_LED, OUTPUT);
  
  // Initialize Serial
  Serial.begin(SERIAL_SPEED);
  
  LOG("start");ENDL;
  
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    LOG(".");
  }
  LOG("WiFi connected");ENDL;

  
  CPU.add(&thread_battery_checker);
}

void battery_checker_run(){
  System::battery_state = !digitalRead(BAT_SENSOR);
}