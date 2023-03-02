#include "Piece.h"

Piece::Piece(Side side, Type type) {
    this->side = side;
    this->type = type;
    maxHealth = maxHealthByType(type);
    health = maxHealth;
}

Piece::Piece(Side side, Type type, uint8 health) : Piece(side, type) {
    this->health = health;
}

uint8 Piece::maxHealthByType(Type type) {
    switch (type) {
    case Type::King:
        return 4;
    case Type::Knight:
        return 3;
    case Type::Archer:
        return 3;
    case Type::Medic:
        return 3;
    case Type::Wizard:
        return 3;
    case Type::Shield:
        return 4;
    }
    return 0;
}
