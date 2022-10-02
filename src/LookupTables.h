#pragma once

#include <array>
#include <vector>
#include <iostream>

#include "Common.h"

using std::array, std::vector, std::cout, std::endl;

extern bool setup;

extern byte bitCount[65536];

extern vector<array<byte, 2>> switchPairs[65536];

extern ushort meleeTargets[65536];

extern ushort archerTargets[65536][17];

extern byte compressedKnights[65536];

extern byte compressedKnightTargets[65536][256];

extern vector<array<byte, 2>> knightTargetPairs[256][256];

extern byte idx[65536];

void setBitCount();

void setSwitchPairs();

void setMeleeTargets();

void setArcherTargets();

void setCompressedKnights();

void setCompressedKnightTargets();

void setKnightTargetPairs();

void setIndex();

void setUp();
