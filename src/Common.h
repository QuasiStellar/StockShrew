#pragma once

#include <cstdint>

using uint8 = uint_fast8_t;
using uint16 = uint_fast16_t;
using uint32 = uint_fast32_t;
using uint64 = uint_fast64_t;

const int MAX_SWAP_COUNT = 21; // 20 possible swaps in a position + 1 for the ending zero
const int MAX_SWAP_COUNT_FULL = 25; // 24 theortically possible swaps (for the swapPairs array) + 1 for the ending zero
const int MAX_ACT_COUNT = 38; // 37 possible actions in a position (6 doubles, 15 heals, 8 hits, 7 wizard swaps and a skip) + 1 for the ending zero
const int MAX_DOUBLE_HIT_COUNT = 12;
