#pragma once

#include <functional>

#include "Common.h"

using std::hash, std::size_t;

struct StateRecord {
    uint16 black;
    uint16 white;
    uint16 active;
    uint16 hp1;
    uint16 hp2;
    uint16 hp3;
    uint16 hp4;
    uint16 kings;
    uint16 knights;
    uint16 archers;
    uint16 medics;
    uint16 wizards;
    uint16 shields;
    uint8 stateInfo;

    StateRecord(uint16 black, uint16 white,
          uint16 hp1, uint16 hp2, uint16 hp3, uint16 hp4,
          uint16 kings, uint16 knights, uint16 archers, uint16 medics, uint16 wizards, uint16 shields,
          uint8 stateInfo);

    bool operator==(const StateRecord &other) const {
        return black == other.black 
        && white == other.white
        && hp1 == other.hp1
        && hp2 == other.hp2
        && hp3 == other.hp3
        && hp4 == other.hp4
        && kings == other.kings
        && knights == other.knights
        && archers == other.archers
        && medics == other.medics
        && wizards == other.wizards
        && shields == other.shields
        && stateInfo == other.stateInfo;
    }
};

template<>
struct std::hash<StateRecord>
{
    size_t operator()(StateRecord const& record) const noexcept
    {
        return record.black ^ record.hp1 ^ record.archers ^ record.medics
         + ((record.white ^ record.hp2 ^ record.wizards) << 16)
          + ((uint64)(record.kings ^ record.hp3 ^ record.shields) << 32)
           + ((uint64)(record.knights ^ record.hp4 ^ record.stateInfo) << 48);
    }
};
