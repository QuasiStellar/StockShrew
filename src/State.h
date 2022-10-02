#pragma once

#include <string>
#include <bitset>
#include <array>
#include <vector>
#include <iostream>

#include "Common.h"
#include "Side.h"
#include "Piece.h"
#include "Phase.h"

struct State {

public:
    ushort black;
    ushort white;
    ushort active;
    ushort superactive;
    ushort hyperactive;
    ushort faraway;
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
    ///
    /// get side:    _stateInfo & 0b1
    /// get phase:   (_stateInfo >> 1) & 0b1
    /// get skips b: (_stateInfo >> 2) & 0b111
    /// get skips w: (_stateInfo >> 5) & 0b111
    byte stateInfo;

    State(Piece* board[4][4], Side side, Phase phase, byte blackSkips, byte whiteSkips);

    float search(byte depth);

    std::vector<State> getOffsprings();

    static void setUp();

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

    [[nodiscard]] State swapPieces(byte piece1, byte piece2) const;

    [[nodiscard]] State damage(ushort piece) const;

    [[nodiscard]] State damage(ushort piece1, ushort piece2) const;

    [[nodiscard]] State heal(ushort piece) const;

    [[nodiscard]] State heal(ushort piece1, ushort piece2) const;

    [[nodiscard]] State heal(ushort piece1, ushort piece2, ushort piece3) const;

    [[nodiscard]] State heal(ushort piece1, ushort piece2, ushort piece3, ushort piece4) const;

    static void setBitCount();

    static void setSwitchPairs();

    static void setMeleeTargets();

    static void setArcherTargets();

    static void setCompressedKnights();

    static void setCompressedKnightTargets();

    static void setKnightTargetPairs();

    static void setIndex();
};
