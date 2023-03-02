#include "StateRecord.h"

StateRecord::StateRecord(uint16 black, uint16 white,
             uint16 hp1, uint16 hp2, uint16 hp3, uint16 hp4,
             uint16 kings, uint16 knights, uint16 archers, uint16 medics, uint16 wizards, uint16 shields,
             uint8 stateInfo)
        : black(black),
          white(white),
          hp1(hp1),
          hp2(hp2),
          hp3(hp3),
          hp4(hp4),
          kings(kings),
          knights(knights),
          archers(archers),
          medics(medics),
          wizards(wizards),
          shields(shields),
          stateInfo(stateInfo) {};
