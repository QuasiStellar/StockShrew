#pragma once

#include "Common.h"
#include "Side.h"
#include "Type.h"

struct Piece {

    Side side;
    Type type;
    uint8 maxHealth;
    uint8 health;

    Piece(Side, Type);

    Piece(Side, Type, uint8);

    static uint8 maxHealthByType(Type);
};
