#pragma once

#include <string>
#include <bitset>
#include <array>
#include <vector>
#include <iostream>

#include "Common.h"
#include "Evaluation.h"
#include "LookupTables.h"
#include "Phase.h"
#include "Piece.h"
#include "Side.h"
#include "Strategy.h"

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
    /// [ 0 0 0 ] [ 0 0 0 ] [ 0 ] [ 0 ]
    /// skips w   skips b   phase side
    /// 000       000       0-1   0-1
    /// 001       001       Swap  Black
    /// 011       011       Act   White
    /// 111       111
    byte stateInfo;

    State(Piece *board[4][4], Side side, Phase phase, byte blackSkips, byte whiteSkips);

    float search(byte depth, Strategy strategy);

    std::vector<State> getOffsprings();

    static long long stateCount;

private:
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

    [[nodiscard]] State swapPieces(byte piece1, byte piece2) const;

    [[nodiscard]] State damage(ushort piece, byte skipMask) const;

    [[nodiscard]] State damage(ushort piece1, ushort piece2, byte skipMask) const;

    [[nodiscard]] State heal(ushort piece, byte skipMask) const;

    [[nodiscard]] State heal(ushort piece1, ushort piece2, byte skipMask) const;

    [[nodiscard]] State heal(ushort piece1, ushort piece2, ushort piece3, byte skipMask) const;

    [[nodiscard]] State heal(ushort piece1, ushort piece2, ushort piece3, ushort piece4, byte skipMask) const;

    [[nodiscard]] State wizardSwap(byte piece1, byte piece2, byte skipMask) const;
};
