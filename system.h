#include <Thread.h>
#include <ThreadController.h>

#include "_config.h"

#ifndef SYSTEM_H
#define SYSTEM_H

extern ThreadController CPU;

class System{
public:
  static void init();
  static bool battery_state;
};
#endif