#pragma once

#include <array>
#include <vector>
#include <iostream>
#include <bitset>

#include "Common.h"

using std::array, std::vector, std::cout, std::endl;

extern bool setup;

/// parameter - bitboard
/// number of set bits in the bitboard
extern byte bitCount[65536];

/// parameter - bitboard of your active pieces
/// vector of index pairs
extern vector<array<byte, 2>> swapPairs[65536];

/// parameter - melee attackers bitboard
/// bitmap of cells under attack
extern ushort meleeTargets[65536];

/// 1st parameter - archers bitboard
/// 2nd parameter - shield
/// bitmap of cells under attack
extern ushort archerTargets[65536][17];

/// parameter - knight bitboard
/// n % 16 - first knight
/// n / 16 - second knight
extern byte compressedKnights[65536];

/// 1st parameter - target bitboard
/// 2nd parameter - compressed knights
/// 1 << 7 - first knight up
/// 1 << 6 - first knight down
/// 1 << 5 - first knight left
/// 1 << 4 - first knight right
/// 1 << 3 - second knight up
/// 1 << 2 - second knight down
/// 1 << 1 - second knight left
/// 1 << 0 - second knight right
extern byte compressedKnightTargets[65536][256];

/// 1st parameter - compressed knight targets
/// 2nd parameter - compressed knights
/// vector knight target pairs
extern vector<array<byte, 2>> knightTargetPairs[256][256];

/// parameter - bitboard with 1 set bit
/// index of the set bit
extern byte idx[65536];

void setBitCount();

void setSwapPairs();

void setMeleeTargets();

void setArcherTargets();

void setCompressedKnights();

void setCompressedKnightTargets();

void setKnightTargetPairs();

void setIndex();

void setUp();
