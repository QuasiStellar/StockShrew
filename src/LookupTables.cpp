#include "LookupTables.h"

using std::bitset, std::mt19937, std::uniform_int_distribution;

bool setup = false;

uint8 bitCount[65536];

vector<array<uint8, 2>> swapPairs[65536];

uint16 meleeTargets[65536];

uint16 archerTargets[65536][17];

uint8 compressedKnights[65536];

uint8 compressedKnightTargets[65536][256];

vector<array<uint8, 2>> knightTargetPairs[256][256];

uint8 idx[65536];

uint32 pieceHashes[64];
uint32 stateInfoHashes[64];

void setBitCount() {
    bitCount[0] = 0;
    for (int i = 0; i < 65536; i++) {
        bitCount[i] = (i & 1) + bitCount[i / 2];
    }
}

void setSwapPairs() {
    for (int i = 0; i < 65536; i++) {
        swapPairs[i] = vector<array<uint8, 2>>();
        for (int j = 0; j < 16; j++) {
            if ((1 << j) & i) {
                if (j + 4 < 16) {
                    swapPairs[i].push_back(array<uint8, 2>{(uint8) j, (uint8) (j + 4)});
                }
                if (!(1 << (j - 4) & i) && j - 4 > -1) {
                    swapPairs[i].push_back(array<uint8, 2>{(uint8) j, (uint8) (j - 4)});
                }
                if (j % 4 != 3) {
                    swapPairs[i].push_back(array<uint8, 2>{(uint8) j, (uint8) (j + 1)});
                }
                if (!(1 << (j - 1) & i) && j % 4 != 0) {
                    swapPairs[i].push_back(array<uint8, 2>{(uint8) j, (uint8) (j - 1)});
                }
            }
        }
    }
}

void setMeleeTargets() {
    for (int i = 0; i < 65536; i++) {
        uint16 map = 0;
        for (int cell = 0; cell < 16; cell++) {
            if ((i & (1 << cell)) == 0) continue;
            int row = cell / 4;
            if (cell + 4 < 16) map |= 1 << (cell + 4);
            if (cell - 4 >= 0) map |= 1 << (cell - 4);
            if ((cell + 1) / 4 == row) map |= 1 << (cell + 1);
            if ((cell - 1) / 4 == row) map |= 1 << (cell - 1);
        }
        meleeTargets[i] = map;
    }
}

void setArcherTargets() {
    for (int i = 0; i < 65536; i++) {
        for (int shield = 0; shield < 17; shield++) {
            uint16 map = 0;
            for (int cell = 0; cell < 16; cell++) {
                if ((i & (1 << cell)) == 0) continue;
                int row = cell / 4;
                for (int what = 0; what < 1; what++) {
                    if (cell + 4 >= 16) break;
                    if (cell + 4 == shield) break;
                    if (cell + 8 < 16) map |= 1 << (cell + 8);
                    else break;
                    if (cell + 8 == shield) break;
                    if (cell + 12 < 16) map |= 1 << (cell + 12);
                }
                for (int what = 0; what < 1; what++) {
                    if (cell - 4 < 0) break;
                    if (cell - 4 == shield) break;
                    if (cell - 8 >= 0) map |= 1 << (cell - 8);
                    else break;
                    if (cell - 8 == shield) break;
                    if (cell - 12 >= 0) map |= 1 << (cell - 12);
                }
                for (int what = 0; what < 1; what++) {
                    if ((cell + 1) / 4 != row) break;
                    if (cell + 1 == shield) break;
                    if ((cell + 2) / 4 == row) map |= 1 << (cell + 2);
                    else break;
                    if (cell + 2 == shield) break;
                    if ((cell + 3) / 4 == row) map |= 1 << (cell + 3);
                }
                for (int what = 0; what < 1; what++) {
                    if ((cell - 1) / 4 != row) break;
                    if (cell - 1 == shield) break;
                    if ((cell - 2) / 4 == row) map |= 1 << (cell - 2);
                    else break;
                    if (cell - 2 == shield) break;
                    if ((cell - 3) / 4 == row) map |= 1 << (cell - 3);
                }
            }
            archerTargets[i][shield] = map;
        }
    }
}

void setCompressedKnights() {
    for (int i = 0; i < 16; i++) {
        compressedKnights[1 << i] = i * 16 + i;
    }
    for (int i = 0; i < 16; i++) {
        for (int j = i + 1; j < 16; j++) {
            compressedKnights[(1 << i) + (1 << j)] = i * 16 + j;
        }
    }
}

void setCompressedKnightTargets() {
    for (int i = 0; i < 65536; i++) {
        for (int j = 0; j < 256; j++) {
            uint8 firstKnight = j % 16;
            uint8 secondKnight = j / 16;
            compressedKnightTargets[i][j] =
                    (firstKnight + 4 < 16 && (1 << (firstKnight + 4)) & i ? 1 << 7 : 0)
                    + (firstKnight - 4 > -1 && (1 << (firstKnight - 4)) & i ? 1 << 6 : 0)
                    + (firstKnight % 4 != 0 && (1 << (firstKnight - 1)) & i ? 1 << 5 : 0)
                    + (firstKnight % 4 != 3 && (1 << (firstKnight + 1)) & i ? 1 << 4 : 0)
                    + (firstKnight == secondKnight ? 0 :
                       ((secondKnight + 4 < 16 && (1 << (secondKnight + 4)) & i ? 1 << 3 : 0)
                        + (secondKnight - 4 > -1 && (1 << (secondKnight - 4)) & i ? 1 << 2 : 0)
                        + (secondKnight % 4 != 0 && (1 << (secondKnight - 1)) & i ? 1 << 1 : 0)
                        + (secondKnight % 4 != 3 && (1 << (secondKnight + 1)) & i ? 1 << 0 : 0)));
        }
    }
}

void setKnightTargetPairs() {
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            knightTargetPairs[i][j] = vector<array<uint8, 2>>();
            uint8 firstKnight = j % 16;
            uint8 secondKnight = j / 16;
            if (i & (1 << 7) && i & (1 << 6)) {
                knightTargetPairs[i][j].push_back(array<uint8, 2>{(uint8) (firstKnight + 4), (uint8) (firstKnight - 4)});
            }
            if (i & (1 << 7) && i & (1 << 5)) {
                knightTargetPairs[i][j].push_back(array<uint8, 2>{(uint8) (firstKnight + 4), (uint8) (firstKnight - 1)});
            }
            if (i & (1 << 7) && i & (1 << 4) && secondKnight - firstKnight != 5) {
                knightTargetPairs[i][j].push_back(array<uint8, 2>{(uint8) (firstKnight + 4), (uint8) (firstKnight + 1)});
            }
            if (i & (1 << 6) && i & (1 << 5)) {
                knightTargetPairs[i][j].push_back(array<uint8, 2>{(uint8) (firstKnight - 4), (uint8) (firstKnight - 1)});
            }
            if (i & (1 << 6) && i & (1 << 4) && firstKnight - secondKnight != 3) {
                knightTargetPairs[i][j].push_back(array<uint8, 2>{(uint8) (firstKnight - 4), (uint8) (firstKnight + 1)});
            }
            if (i & (1 << 5) && i & (1 << 4)) {
                knightTargetPairs[i][j].push_back(array<uint8, 2>{(uint8) (firstKnight - 1), (uint8) (firstKnight + 1)});
            }
            if (i & (1 << 3) && i & (1 << 2)) {
                knightTargetPairs[i][j].push_back(array<uint8, 2>{(uint8) (secondKnight + 4), (uint8) (secondKnight - 4)});
            }
            if (i & (1 << 3) && i & (1 << 1)) {
                knightTargetPairs[i][j].push_back(array<uint8, 2>{(uint8) (secondKnight + 4), (uint8) (secondKnight - 1)});
            }
            if (i & (1 << 3) && i & (1 << 0) && firstKnight - secondKnight != 5) {
                knightTargetPairs[i][j].push_back(array<uint8, 2>{(uint8) (secondKnight + 4), (uint8) (secondKnight + 1)});
            }
            if (i & (1 << 2) && i & (1 << 1)) {
                knightTargetPairs[i][j].push_back(array<uint8, 2>{(uint8) (secondKnight - 4), (uint8) (secondKnight - 1)});
            }
            if (i & (1 << 2) && i & (1 << 0) && secondKnight - firstKnight != 3) {
                knightTargetPairs[i][j].push_back(array<uint8, 2>{(uint8) (secondKnight - 4), (uint8) (secondKnight + 1)});
            }
            if (i & (1 << 1) && i & (1 << 0)) {
                knightTargetPairs[i][j].push_back(array<uint8, 2>{(uint8) (secondKnight - 1), (uint8) (secondKnight + 1)});
            }
        }
    }
}

void setIndex() {
    for (int i = 1; i < 65536; i++) {
        if ((i & (i - 1)) != 0) continue;
        int j = i;
        for (; j != 1; idx[i]++) j >>= 1;
    }
}

void setHashes() {
    mt19937 generator(0); // NOLINT(cert-msc51-cpp)
    uniform_int_distribution<uint32> range(0, UINT_MAX);
    for (uint32 & pieceHash : pieceHashes) {
        pieceHash = range(generator);
    }
    for (uint32 &stateInfoHash : stateInfoHashes) {
        stateInfoHash = range(generator);
    }
}

void setUp() {
    if (setup) {
        return;
    }
    setBitCount();
    setSwapPairs();
    setMeleeTargets();
    setArcherTargets();
    setCompressedKnights();
    setCompressedKnightTargets();
    setKnightTargetPairs();
    setIndex();
    setHashes();
    setup = true;
}
