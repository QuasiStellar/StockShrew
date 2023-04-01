#pragma once

#include <array>
#include <iostream>
#include <bitset>
#include <random>
#include <unordered_map>

#include "Common.h"

using std::array;

extern bool setup;

/// parameter - bitboard
/// number of set bits in the bitboard
extern uint8 bitCount[65536];

/// parameter - bitboard of your active pieces
/// array of index pairs
extern array<array<uint8, 2>, MAX_SWAP_COUNT_FULL> swapPairs[65536];

/// parameter - melee attackers bitboard
/// bitmap of cells under attack
extern uint16 meleeTargets[65536];

/// 1st parameter - archers bitboard
/// 2nd parameter - shield
/// bitmap of cells under attack
extern uint16 archerTargets[65536][17];

/// parameter - knight bitboard
/// n % 16 - first knight
/// n / 16 - second knight
extern uint8 compressedKnights[65536];

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
extern uint8 compressedKnightTargets[65536][256];

/// 1st parameter - compressed knight targets
/// 2nd parameter - compressed knights
/// array of knight target pairs
extern array<array<uint8, 2>, MAX_DOUBLE_HIT_COUNT> knightTargetPairs[256][256];

/// parameter - bitboard with 1 set bit
/// index of the set bit
extern uint8 idx[65536];

extern uint32 pieceHashes[64];

extern uint32 stateInfoHashes[64];

void setBitCount();

void setSwapPairs();

void setMeleeTargets();

void setArcherTargets();

void setCompressedKnights();

void setCompressedKnightTargets();

void setKnightTargetPairs();

void setIndex();

void setHashes();

void setUp();
