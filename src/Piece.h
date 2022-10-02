#pragma once

#include "Common.h"
#include "Side.h"
#include "Type.h"

class Piece {

public:
    Side side;
    Type type;
    byte maxHealth;
    byte health;

    Piece(Side, Type);

    Piece(Side, Type, byte);

private:
    static byte maxHealthByType(Type);
};
