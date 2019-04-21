#include <Thread.h>
#include <ThreadController.h>

#include "_config.h"

#include "system.h"
#include "sensors.h"
#include "communication.h"

void setup(){
  System::init();  
  Sensors::init();
  Communication::init();
}

void loop(){
  CPU.run();
}