#pragma once

#include <string>
#include <bitset>
#include <array>
#include <vector>
#include <iostream>
#include <utility>

#include "Common.h"
#include "Display.h"
#include "Evaluation.h"
#include "LookupTables.h"
#include "Phase.h"
#include "Piece.h"
#include "Side.h"
#include "Strategy.h"

using std::vector, std::pair, std::string;

struct State {

public:
    ushort black;
    ushort white;
    ushort active;
    ushort hp1;
    ushort hp2;
    ushort hp3;
    ushort hp4;
    ushort kings;
    ushort knights;
    ushort archers;
    ushort medics;
    ushort wizards;
    ushort shields;

    /// 1 BYTE
    /// [ 0 0 ] [ 0 0 ] [ 0 ] [ 0 ]
    /// skips w skips b phase side
    /// 00      00      0-1   0-1
    /// 01      01      Swap  Black
    /// 10      10      Act   White
    /// 11      11
    byte stateInfo;

    State(Piece *board[4][4], Side side, Phase phase, byte blackSkips, byte whiteSkips);

    float search(byte depth, Strategy strategy);

    [[nodiscard]] vector<pair<State, string>> getOffsprings() const;

    [[nodiscard]] vector<byte> getBlackSwapMoves() const;

    [[nodiscard]] vector<byte> getWhiteSwapMoves() const;

    [[nodiscard]] vector<int> getBlackActMoves() const;

    [[nodiscard]] vector<int> getWhiteActMoves() const;

    static long long stateCount;

//private:
    State(ushort black, ushort white,
          ushort hp1, ushort hp2, ushort hp3, ushort hp4,
          ushort kings, ushort knights, ushort archers, ushort medics, ushort wizards, ushort shields,
          byte stateInfo);

    [[nodiscard]] int endGameScore() const;

    [[nodiscard]] float evaluate() const;

    /// black aims for high score
    /// white aims for low  score
    ///
    /// alpha - highest score black can guarantee
    /// beta  - lowest  score white can guarantee
    ///
    /// __alpha_____real_____beta__
    float abBlackSwap(float alpha, float beta, byte depth);

    float abBlackSwap(byte depth);

    float abBlackAct(float alpha, float beta, byte depth);

    float abBlackAct(byte depth);

    float abWhiteSwap(float alpha, float beta, byte depth);

    float abWhiteSwap(byte depth);

    float abWhiteAct(float alpha, float beta, byte depth);

    float abWhiteAct(byte depth);

#pragma region MiniMax

    float mmBlackSwap(byte depth);

    float mmBlackAct(byte depth);

    float mmWhiteSwap(byte depth);

    float mmWhiteAct(byte depth);

#pragma endregion

    inline void swap(byte piece1, byte piece2);

    inline void makeSwap(byte swapMask);

    inline void makeBlackAct(int actMask);

    inline void makeWhiteAct(int actMask);

    inline void unmakeBlackAct(int actMask);

    inline void unmakeWhiteAct(int actMask);
};
