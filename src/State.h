#pragma once

#include <string>
#include <bitset>
#include <array>
#include <vector>
#include <iostream>
#include <utility>
#include <unordered_map>
#include <unordered_set>

#include "Common.h"
#include "Display.h"
#include "Evaluation.h"
#include "LookupTables.h"
#include "Phase.h"
#include "Piece.h"
#include "Side.h"
#include "StateRecord.h"
#include "Strategy.h"

using std::vector, std::pair, std::string, std::unordered_map, std::unordered_set;

struct State {

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

    /// 1 BYTE
    /// [ 0 0 ] [ 0 0 ] [ 0 ] [ 0 ]
    /// skips w skips b phase side
    /// 00      00      0-1   0-1
    /// 01      01      Swap  Black
    /// 10      10      Act   White
    /// 11      11
    uint8 stateInfo;

    State(Piece *board[4][4], Side side, Phase phase, uint8 blackSkips, uint8 whiteSkips);

    State(uint16 black, uint16 white,
          uint16 hp1, uint16 hp2, uint16 hp3, uint16 hp4,
          uint16 kings, uint16 knights, uint16 archers, uint16 medics, uint16 wizards, uint16 shields,
          uint8 stateInfo);

    float search(uint8 depth, Strategy strategy);

    [[nodiscard]] vector<pair<State, string>> getOffsprings() const;

    [[nodiscard]] vector<uint8> getBlackSwapMoves() const;

    [[nodiscard]] vector<uint8> getWhiteSwapMoves() const;

    [[nodiscard]] vector<int> getBlackActMoves() const;

    [[nodiscard]] vector<int> getWhiteActMoves() const;

    static long long stateCount;

    static unordered_set<StateRecord> history;

    [[nodiscard]] int endGameScore() const;

    [[nodiscard]] float evaluate() const;

    /// black aims for high score
    /// white aims for low  score
    ///
    /// alpha - highest score black can guarantee
    /// beta  - lowest  score white can guarantee
    ///
    /// __alpha_____real_____beta__
    float abBlackSwap(float alpha, float beta, uint8 depth);

    float abBlackSwap(uint8 depth);

    float abBlackAct(float alpha, float beta, uint8 depth);

    float abBlackAct(uint8 depth);

    float abWhiteSwap(float alpha, float beta, uint8 depth);

    float abWhiteSwap(uint8 depth);

    float abWhiteAct(float alpha, float beta, uint8 depth);

    float abWhiteAct(uint8 depth);

    float mmBlackSwap(uint8 depth);

    float mmBlackAct(uint8 depth);

    float mmWhiteSwap(uint8 depth);

    float mmWhiteAct(uint8 depth);

    void swap(uint8 piece1, uint8 piece2);

    void updateExtraBitboards();

    void makeSwap(uint8 swapMask);

    /// actMask:
    ///
    /// type:
    /// 00 - skip
    /// 01 - wizard swap
    /// 10 - attack
    /// 11 - heal
    ///
    /// skip
    /// [ 0 ]
    /// type
    ///
    /// wizard swap
    /// [ 0 0 0 0 ] [ 0 0 0 0 ] [ 0 0 ] [ 0 0 ]
    /// first index second index skips  type
    ///
    /// damage
    /// [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ] [ 0 0 ] [ 0 0 ]
    /// mask                                skips   type
    ///
    /// heal
    /// [ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ] [ 0 0 ] [ 0 0 ]
    /// mask                                skips   type

    void makeBlackAct(int actMask);

    void makeWhiteAct(int actMask);

    void unmakeBlackAct(int actMask);

    void unmakeWhiteAct(int actMask);

    [[nodiscard]] string display() const;

    /// Call after each search.
    static void resetGlobals();
};
