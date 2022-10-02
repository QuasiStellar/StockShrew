#pragma once

#include "Piece.h"
#include "State.h"

Piece* p(const char* input) {
    Type type;
    if (input[0] == 'k' || input[0] == 'K') type = Type::King;
    else if (input[0] == 'n' || input[0] == 'N') type = Type::Knight;
    else if (input[0] == 'a' || input[0] == 'A') type = Type::Archer;
    else if (input[0] == 'm' || input[0] == 'M') type = Type::Medic;
    else if (input[0] == 'w' || input[0] == 'W') type = Type::Wizard;
    else if (input[0] == 's' || input[0] == 'S') type = Type::Shield;
    else return nullptr;
    int hp;
    if (input[1] == '1') hp = 1;
    else if (input[1] == '2') hp = 2;
    else if (input[1] == '3') hp = 3;
    else if (input[1] == '4') hp = 4;
    else return nullptr;
    Side side = input[0] < 95 ? Side::Black : Side::White;
    return new Piece(side, type, hp);
}
