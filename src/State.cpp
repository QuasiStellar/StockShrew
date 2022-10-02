#include "State.h"

using std::cout, std::endl, std::array, std::vector;

const byte BLACK = 0b0;
const byte WHITE = 0b1;
const byte SWAP = 0b0;
const byte ACT = 0b1;

const ushort NOT_LEFTMOST = 0b0111011101110111;
const ushort NOT_RIGHTMOST = 0b1110111011101110;
const ushort NOT_LEFT_HALF = 0b0011001100110011;
const ushort NOT_RIGHT_HALF = 0b1100110011001100;

long long State::stateCount;

State::State(Piece *board[4][4], Side side, Phase phase, byte blackSkips, byte whiteSkips) {

    black = white = hp1 = hp2 = hp3 = hp4 = kings = knights = archers = medics = wizards = shields = 0;

    for (byte i = 0; i < 4; i++) {
        for (byte j = 0; j < 4; j++) {
            if (board[3 - i][j] == nullptr) continue;

            switch (board[3 - i][j]->side) {
                case Side::Black:
                    black += 1 << (i * 4 + j);
                    break;
                case Side::White:
                    white += 1 << (i * 4 + j);
                    break;
            }

            switch (board[3 - i][j]->health) {
                case 1:
                    hp1 += 1 << (i * 4 + j);
                    break;
                case 2:
                    hp2 += 1 << (i * 4 + j);
                    break;
                case 3:
                    hp3 += 1 << (i * 4 + j);
                    break;
                case 4:
                    hp4 += 1 << (i * 4 + j);
                    break;
            }

            switch (board[3 - i][j]->type) {
                case Type::King:
                    kings += 1 << (i * 4 + j);
                    break;
                case Type::Knight:
                    knights += 1 << (i * 4 + j);
                    break;
                case Type::Archer:
                    archers += 1 << (i * 4 + j);
                    break;
                case Type::Medic:
                    medics += 1 << (i * 4 + j);
                    break;
                case Type::Wizard:
                    wizards += 1 << (i * 4 + j);
                    break;
                case Type::Shield:
                    shields += 1 << (i * 4 + j);
                    break;
            }
        }
    }

    active = (this->black & (this->black << 4
                             | this->black >> 4
                             | (this->black << 1) & NOT_RIGHTMOST
                             | (this->black >> 1) & NOT_LEFTMOST))
             | (this->white & (this->white << 4
                               | this->white >> 4
                               | (this->white << 1) & NOT_RIGHTMOST
                               | (this->white >> 1) & NOT_LEFTMOST));

    stateInfo = (int) side + ((int) phase << 1)
                + (blackSkips == 1 ? 0b100 : 0)
                + (blackSkips == 2 ? 0b1100 : 0)
                + (blackSkips == 3 ? 0b11100 : 0)
                + (whiteSkips == 1 ? 0b100000 : 0)
                + (whiteSkips == 2 ? 0b1100000 : 0)
                + (whiteSkips == 3 ? 0b11100000 : 0);
}

State::State(ushort black, ushort white,
             ushort hp1, ushort hp2, ushort hp3, ushort hp4,
             ushort kings, ushort knights, ushort archers, ushort medics, ushort wizards, ushort shields,
             byte stateInfo) {

    this->black = black;
    this->white = white;
    this->hp1 = hp1;
    this->hp2 = hp2;
    this->hp3 = hp3;
    this->hp4 = hp4;
    this->hp4 = hp4;
    this->kings = kings;
    this->knights = knights;
    this->archers = archers;
    this->medics = medics;
    this->wizards = wizards;
    this->shields = shields;
    this->stateInfo = stateInfo;

    active = (this->black & (this->black << 4
                             | this->black >> 4
                             | (this->black << 1) & NOT_RIGHTMOST
                             | (this->black >> 1) & NOT_LEFTMOST))
             | (this->white & (this->white << 4
                               | this->white >> 4
                               | (this->white << 1) & NOT_RIGHTMOST
                               | (this->white >> 1) & NOT_LEFTMOST));
}

float State::search(byte depth, Strategy strategy) {
    switch (strategy) {
        case Strategy::AlphaBeta:
            switch (stateInfo & 0b1) {
                case BLACK:
                    switch ((stateInfo >> 1) & 0b1) {
                        case SWAP:
                            return abBlackSwap(depth);
                        case ACT:
                            return abBlackAct(depth);
                    }
                case WHITE:
                    switch ((stateInfo >> 1) & 0b1) {
                        case SWAP:
                            return abWhiteSwap(depth);
                        case ACT:
                            return abWhiteAct(depth);
                    }
            }
        case Strategy::MiniMax:
            switch (stateInfo & 0b1) {
                case BLACK:
                    switch ((stateInfo >> 1) & 0b1) {
                        case SWAP:
                            return mmBlackSwap(depth);
                        case ACT:
                            return mmBlackAct(depth);
                    }
                case WHITE:
                    switch ((stateInfo >> 1) & 0b1) {
                        case SWAP:
                            return mmWhiteSwap(depth);
                        case ACT:
                            return mmWhiteAct(depth);
                    }
            }
    }
    return 0;
}

int State::endGameScore() const {
    if ((stateInfo >> 4) & 1) return INT_MIN; // black is out of skips
    if ((stateInfo >> 7) & 1) return INT_MAX; // white is out of skips
    bool blackKing = (black & kings) != 0;
    bool whiteKing = (white & kings) != 0;
    bool blackActive = (black & active) != 0;
    bool whiteActive = (white & active) != 0;
    if (!blackActive && !whiteActive) return 0;
    if (!blackKing || !blackActive) return INT_MIN;
    if (!whiteKing || !whiteActive) return INT_MAX;
    return -1;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-narrowing-conversions"

float State::evaluate() const {

    ushort blackUp = this->black << 4;
    ushort blackDown = this->black >> 4;
    ushort blackRight = (this->black << 1) & NOT_RIGHTMOST;
    ushort blackLeft = (this->black >> 1) & NOT_LEFTMOST;

    ushort whiteUp = this->white << 4;
    ushort whiteDown = this->white >> 4;
    ushort whiteRight = (this->white << 1) & NOT_RIGHTMOST;
    ushort whiteLeft = (this->white >> 1) & NOT_LEFTMOST;

    ushort superactive = (this->black & ((blackUp & blackDown)
                                         | (blackRight & blackLeft)
                                         | (blackUp & blackRight)
                                         | (blackRight & blackDown)
                                         | (blackDown & blackLeft)
                                         | (blackLeft & blackUp)))
                         | (this->white & ((whiteUp & whiteDown)
                                           | (whiteRight & whiteLeft)
                                           | (whiteUp & whiteRight)
                                           | (whiteRight & whiteDown)
                                           | (whiteDown & whiteLeft)
                                           | (whiteLeft & whiteUp)));

    ushort hyperactive = (this->black & ((blackUp & blackRight & blackLeft)
                                         | (blackDown & blackRight & blackLeft)
                                         | (blackRight & blackUp & blackDown)
                                         | (blackLeft & blackUp & blackDown)))
                         | (this->white & ((whiteUp & whiteRight & whiteLeft)
                                           | (whiteDown & whiteRight & whiteLeft)
                                           | (whiteRight & whiteUp & whiteDown)
                                           | (whiteLeft & whiteUp & whiteDown)));

    ushort blackActive = this->black & active;
    ushort whiteActive = this->white & active;

    ushort faraway = (this->black & ~((blackActive << 4)
                                      | (blackActive >> 4)
                                      | ((blackActive << 1) & NOT_RIGHTMOST)
                                      | ((blackActive >> 1) & NOT_LEFTMOST)
                                      | ((blackActive << 5) & NOT_RIGHTMOST)
                                      | ((blackActive << 3) & NOT_LEFTMOST)
                                      | ((blackActive >> 5) & NOT_LEFTMOST)
                                      | ((blackActive >> 3) & NOT_RIGHTMOST)
                                      | (blackActive << 8)
                                      | (blackActive >> 8)
                                      | ((blackActive << 2) & NOT_RIGHT_HALF)
                                      | ((blackActive >> 2) & NOT_LEFT_HALF)))
                     | (this->white & ~((whiteActive << 4)
                                        | (whiteActive >> 4)
                                        | ((whiteActive << 1) & NOT_RIGHTMOST)
                                        | ((whiteActive >> 1) & NOT_LEFTMOST)
                                        | ((whiteActive << 5) & NOT_RIGHTMOST)
                                        | ((whiteActive << 3) & NOT_LEFTMOST)
                                        | ((whiteActive >> 5) & NOT_LEFTMOST)
                                        | ((whiteActive >> 3) & NOT_RIGHTMOST)
                                        | (whiteActive << 8)
                                        | (whiteActive >> 8)
                                        | ((whiteActive << 2) & NOT_RIGHT_HALF)
                                        | ((whiteActive >> 2) & NOT_LEFT_HALF)));

    return ((stateInfo & 0b1) == BLACK && ((stateInfo >> 1) & 0b1) == SWAP ? SWAP_ADVANTAGE : 0)
           - ((stateInfo & 0b1) == WHITE && ((stateInfo >> 1) & 0b1) == SWAP ? SWAP_ADVANTAGE : 0)
           + KING1_HP_FARAWAY * (bitCount[hp1 & black & kings & faraway]
                                 - bitCount[hp1 & white & kings & faraway])
           + KING1_HP_INACTIVE * (bitCount[hp1 & black & kings & ~active & ~faraway]
                                  - bitCount[hp1 & white & kings & ~active & ~faraway])
           + KING1_HP_ACTIVE * (bitCount[hp1 & black & kings & active & ~superactive]
                                - bitCount[hp1 & white & kings & active & ~superactive])
           + KING1_HP_SUPERACTIVE * (bitCount[hp1 & black & kings & superactive & ~hyperactive]
                                     - bitCount[hp1 & white & kings & superactive & ~hyperactive])
           + KING1_HP_HYPERACTIVE * (bitCount[hp1 & black & kings & hyperactive]
                                     - bitCount[hp1 & white & kings & hyperactive])
           + KING2_HP_FARAWAY * (bitCount[hp2 & black & kings & faraway]
                                 - bitCount[hp2 & white & kings & faraway])
           + KING2_HP_INACTIVE * (bitCount[hp2 & black & kings & ~active & ~faraway]
                                  - bitCount[hp2 & white & kings & ~active & ~faraway])
           + KING2_HP_ACTIVE * (bitCount[hp2 & black & kings & active & ~superactive]
                                - bitCount[hp2 & white & kings & active & ~superactive])
           + KING2_HP_SUPERACTIVE * (bitCount[hp2 & black & kings & superactive & ~hyperactive]
                                     - bitCount[hp2 & white & kings & superactive & ~hyperactive])
           + KING2_HP_HYPERACTIVE * (bitCount[hp2 & black & kings & hyperactive]
                                     - bitCount[hp2 & white & kings & hyperactive])
           + KING3_HP_FARAWAY * (bitCount[hp3 & black & kings & faraway]
                                 - bitCount[hp3 & white & kings & faraway])
           + KING3_HP_INACTIVE * (bitCount[hp3 & black & kings & ~active & ~faraway]
                                  - bitCount[hp3 & white & kings & ~active & ~faraway])
           + KING3_HP_ACTIVE * (bitCount[hp3 & black & kings & active & ~superactive]
                                - bitCount[hp3 & white & kings & active & ~superactive])
           + KING3_HP_SUPERACTIVE * (bitCount[hp3 & black & kings & superactive & ~hyperactive]
                                     - bitCount[hp3 & white & kings & superactive & ~hyperactive])
           + KING3_HP_HYPERACTIVE * (bitCount[hp3 & black & kings & hyperactive]
                                     - bitCount[hp3 & white & kings & hyperactive])
           + KING4_HP_FARAWAY * (bitCount[hp4 & black & kings & faraway]
                                 - bitCount[hp4 & white & kings & faraway])
           + KING4_HP_INACTIVE * (bitCount[hp4 & black & kings & ~active & ~faraway]
                                  - bitCount[hp4 & white & kings & ~active & ~faraway])
           + KING4_HP_ACTIVE * (bitCount[hp4 & black & kings & active & ~superactive]
                                - bitCount[hp4 & white & kings & active & ~superactive])
           + KING4_HP_SUPERACTIVE * (bitCount[hp4 & black & kings & superactive & ~hyperactive]
                                     - bitCount[hp4 & white & kings & superactive & ~hyperactive])
           + KING4_HP_HYPERACTIVE * (bitCount[hp4 & black & kings & hyperactive]
                                     - bitCount[hp4 & white & kings & hyperactive])
           + KNIGHT1_HP_FARAWAY * (bitCount[hp1 & black & knights & faraway]
                                   - bitCount[hp1 & white & knights & faraway])
           + KNIGHT1_HP_INACTIVE * (bitCount[hp1 & black & knights & ~active & ~faraway]
                                    - bitCount[hp1 & white & knights & ~active & ~faraway])
           + KNIGHT1_HP_ACTIVE * (bitCount[hp1 & black & knights & active & ~superactive]
                                  - bitCount[hp1 & white & knights & active & ~superactive])
           + KNIGHT1_HP_SUPERACTIVE * (bitCount[hp1 & black & knights & superactive & ~hyperactive]
                                       - bitCount[hp1 & white & knights & superactive & ~hyperactive])
           + KNIGHT1_HP_HYPERACTIVE * (bitCount[hp1 & black & knights & hyperactive]
                                       - bitCount[hp1 & white & knights & hyperactive])
           + KNIGHT2_HP_FARAWAY * (bitCount[hp2 & black & knights & faraway]
                                   - bitCount[hp2 & white & knights & faraway])
           + KNIGHT2_HP_INACTIVE * (bitCount[hp2 & black & knights & ~active & ~faraway]
                                    - bitCount[hp2 & white & knights & ~active & ~faraway])
           + KNIGHT2_HP_ACTIVE * (bitCount[hp2 & black & knights & active & ~superactive]
                                  - bitCount[hp2 & white & knights & active & ~superactive])
           + KNIGHT2_HP_SUPERACTIVE * (bitCount[hp2 & black & knights & superactive & ~hyperactive]
                                       - bitCount[hp2 & white & knights & superactive & ~hyperactive])
           + KNIGHT2_HP_HYPERACTIVE * (bitCount[hp2 & black & knights & hyperactive]
                                       - bitCount[hp2 & white & knights & hyperactive])
           + KNIGHT3_HP_FARAWAY * (bitCount[hp3 & black & knights & faraway]
                                   - bitCount[hp3 & white & knights & faraway])
           + KNIGHT3_HP_INACTIVE * (bitCount[hp3 & black & knights & ~active & ~faraway]
                                    - bitCount[hp3 & white & knights & ~active & ~faraway])
           + KNIGHT3_HP_ACTIVE * (bitCount[hp3 & black & knights & active & ~superactive]
                                  - bitCount[hp3 & white & knights & active & ~superactive])
           + KNIGHT3_HP_SUPERACTIVE * (bitCount[hp3 & black & knights & superactive & ~hyperactive]
                                       - bitCount[hp3 & white & knights & superactive & ~hyperactive])
           + KNIGHT3_HP_HYPERACTIVE * (bitCount[hp3 & black & knights & hyperactive]
                                       - bitCount[hp3 & white & knights & hyperactive])
           + ARCHER1_HP_FARAWAY * (bitCount[hp1 & black & archers & faraway]
                                   - bitCount[hp1 & white & archers & faraway])
           + ARCHER1_HP_INACTIVE * (bitCount[hp1 & black & archers & ~active & ~faraway]
                                    - bitCount[hp1 & white & archers & ~active & ~faraway])
           + ARCHER1_HP_ACTIVE * (bitCount[hp1 & black & archers & active & ~superactive]
                                  - bitCount[hp1 & white & archers & active & ~superactive])
           + ARCHER1_HP_SUPERACTIVE * (bitCount[hp1 & black & archers & superactive & ~hyperactive]
                                       - bitCount[hp1 & white & archers & superactive & ~hyperactive])
           + ARCHER1_HP_HYPERACTIVE * (bitCount[hp1 & black & archers & hyperactive]
                                       - bitCount[hp1 & white & archers & hyperactive])
           + ARCHER2_HP_FARAWAY * (bitCount[hp2 & black & archers & faraway]
                                   - bitCount[hp2 & white & archers & faraway])
           + ARCHER2_HP_INACTIVE * (bitCount[hp2 & black & archers & ~active & ~faraway]
                                    - bitCount[hp2 & white & archers & ~active & ~faraway])
           + ARCHER2_HP_ACTIVE * (bitCount[hp2 & black & archers & active & ~superactive]
                                  - bitCount[hp2 & white & archers & active & ~superactive])
           + ARCHER2_HP_SUPERACTIVE * (bitCount[hp2 & black & archers & superactive & ~hyperactive]
                                       - bitCount[hp2 & white & archers & superactive & ~hyperactive])
           + ARCHER2_HP_HYPERACTIVE * (bitCount[hp2 & black & archers & hyperactive]
                                       - bitCount[hp2 & white & archers & hyperactive])
           + ARCHER3_HP_FARAWAY * (bitCount[hp3 & black & archers & faraway]
                                   - bitCount[hp3 & white & archers & faraway])
           + ARCHER3_HP_INACTIVE * (bitCount[hp3 & black & archers & ~active & ~faraway]
                                    - bitCount[hp3 & white & archers & ~active & ~faraway])
           + ARCHER3_HP_ACTIVE * (bitCount[hp3 & black & archers & active & ~superactive]
                                  - bitCount[hp3 & white & archers & active & ~superactive])
           + ARCHER3_HP_SUPERACTIVE * (bitCount[hp3 & black & archers & superactive & ~hyperactive]
                                       - bitCount[hp3 & white & archers & superactive & ~hyperactive])
           + ARCHER3_HP_HYPERACTIVE * (bitCount[hp3 & black & archers & hyperactive]
                                       - bitCount[hp3 & white & archers & hyperactive])
           + MEDIC1_HP_FARAWAY * (bitCount[hp1 & black & medics & faraway]
                                  - bitCount[hp1 & white & medics & faraway])
           + MEDIC1_HP_INACTIVE * (bitCount[hp1 & black & medics & ~active & ~faraway]
                                   - bitCount[hp1 & white & medics & ~active & ~faraway])
           + MEDIC1_HP_ACTIVE * (bitCount[hp1 & black & medics & active & ~superactive]
                                 - bitCount[hp1 & white & medics & active & ~superactive])
           + MEDIC1_HP_SUPERACTIVE * (bitCount[hp1 & black & medics & superactive & ~hyperactive]
                                      - bitCount[hp1 & white & medics & superactive & ~hyperactive])
           + MEDIC1_HP_HYPERACTIVE * (bitCount[hp1 & black & medics & hyperactive]
                                      - bitCount[hp1 & white & medics & hyperactive])
           + MEDIC2_HP_FARAWAY * (bitCount[hp2 & black & medics & faraway]
                                  - bitCount[hp2 & white & medics & faraway])
           + MEDIC2_HP_INACTIVE * (bitCount[hp2 & black & medics & ~active & ~faraway]
                                   - bitCount[hp2 & white & medics & ~active & ~faraway])
           + MEDIC2_HP_ACTIVE * (bitCount[hp2 & black & medics & active & ~superactive]
                                 - bitCount[hp2 & white & medics & active & ~superactive])
           + MEDIC2_HP_SUPERACTIVE * (bitCount[hp2 & black & medics & superactive & ~hyperactive]
                                      - bitCount[hp2 & white & medics & superactive & ~hyperactive])
           + MEDIC2_HP_HYPERACTIVE * (bitCount[hp2 & black & medics & hyperactive]
                                      - bitCount[hp2 & white & medics & hyperactive])
           + MEDIC3_HP_FARAWAY * (bitCount[hp3 & black & medics & faraway]
                                  - bitCount[hp3 & white & medics & faraway])
           + MEDIC3_HP_INACTIVE * (bitCount[hp3 & black & medics & ~active & ~faraway]
                                   - bitCount[hp3 & white & medics & ~active & ~faraway])
           + MEDIC3_HP_ACTIVE * (bitCount[hp3 & black & medics & active & ~superactive]
                                 - bitCount[hp3 & white & medics & active & ~superactive])
           + MEDIC3_HP_SUPERACTIVE * (bitCount[hp3 & black & medics & superactive & ~hyperactive]
                                      - bitCount[hp3 & white & medics & superactive & ~hyperactive])
           + MEDIC3_HP_HYPERACTIVE * (bitCount[hp3 & black & medics & hyperactive]
                                      - bitCount[hp3 & white & medics & hyperactive])
           + WIZARD1_HP_FARAWAY * (bitCount[hp1 & black & wizards & faraway]
                                   - bitCount[hp1 & white & wizards & faraway])
           + WIZARD1_HP_INACTIVE * (bitCount[hp1 & black & wizards & ~active & ~faraway]
                                    - bitCount[hp1 & white & wizards & ~active & ~faraway])
           + WIZARD1_HP_ACTIVE * (bitCount[hp1 & black & wizards & active & ~superactive]
                                  - bitCount[hp1 & white & wizards & active & ~superactive])
           + WIZARD1_HP_SUPERACTIVE * (bitCount[hp1 & black & wizards & superactive & ~hyperactive]
                                       - bitCount[hp1 & white & wizards & superactive & ~hyperactive])
           + WIZARD1_HP_HYPERACTIVE * (bitCount[hp1 & black & wizards & hyperactive]
                                       - bitCount[hp1 & white & wizards & hyperactive])
           + WIZARD2_HP_FARAWAY * (bitCount[hp2 & black & wizards & faraway]
                                   - bitCount[hp2 & white & wizards & faraway])
           + WIZARD2_HP_INACTIVE * (bitCount[hp2 & black & wizards & ~active & ~faraway]
                                    - bitCount[hp2 & white & wizards & ~active & ~faraway])
           + WIZARD2_HP_ACTIVE * (bitCount[hp2 & black & wizards & active & ~superactive]
                                  - bitCount[hp2 & white & wizards & active & ~superactive])
           + WIZARD2_HP_SUPERACTIVE * (bitCount[hp2 & black & wizards & superactive & ~hyperactive]
                                       - bitCount[hp2 & white & wizards & superactive & ~hyperactive])
           + WIZARD2_HP_HYPERACTIVE * (bitCount[hp2 & black & wizards & hyperactive]
                                       - bitCount[hp2 & white & wizards & hyperactive])
           + WIZARD3_HP_FARAWAY * (bitCount[hp3 & black & wizards & faraway]
                                   - bitCount[hp3 & white & wizards & faraway])
           + WIZARD3_HP_INACTIVE * (bitCount[hp3 & black & wizards & ~active & ~faraway]
                                    - bitCount[hp3 & white & wizards & ~active & ~faraway])
           + WIZARD3_HP_ACTIVE * (bitCount[hp3 & black & wizards & active & ~superactive]
                                  - bitCount[hp3 & white & wizards & active & ~superactive])
           + WIZARD3_HP_SUPERACTIVE * (bitCount[hp3 & black & wizards & superactive & ~hyperactive]
                                       - bitCount[hp3 & white & wizards & superactive & ~hyperactive])
           + WIZARD3_HP_HYPERACTIVE * (bitCount[hp3 & black & wizards & hyperactive]
                                       - bitCount[hp3 & white & wizards & hyperactive])
           + SHIELD1_HP_FARAWAY * (bitCount[hp1 & black & shields & faraway]
                                   - bitCount[hp1 & white & shields & faraway])
           + SHIELD1_HP_INACTIVE * (bitCount[hp1 & black & shields & ~active & ~faraway]
                                    - bitCount[hp1 & white & shields & ~active & ~faraway])
           + SHIELD1_HP_ACTIVE * (bitCount[hp1 & black & shields & active & ~superactive]
                                  - bitCount[hp1 & white & shields & active & ~superactive])
           + SHIELD1_HP_SUPERACTIVE * (bitCount[hp1 & black & shields & superactive & ~hyperactive]
                                       - bitCount[hp1 & white & shields & superactive & ~hyperactive])
           + SHIELD1_HP_HYPERACTIVE * (bitCount[hp1 & black & shields & hyperactive]
                                       - bitCount[hp1 & white & shields & hyperactive])
           + SHIELD2_HP_FARAWAY * (bitCount[hp2 & black & shields & faraway]
                                   - bitCount[hp2 & white & shields & faraway])
           + SHIELD2_HP_INACTIVE * (bitCount[hp2 & black & shields & ~active & ~faraway]
                                    - bitCount[hp2 & white & shields & ~active & ~faraway])
           + SHIELD2_HP_ACTIVE * (bitCount[hp2 & black & shields & active & ~superactive]
                                  - bitCount[hp2 & white & shields & active & ~superactive])
           + SHIELD2_HP_SUPERACTIVE * (bitCount[hp2 & black & shields & superactive & ~hyperactive]
                                       - bitCount[hp2 & white & shields & superactive & ~hyperactive])
           + SHIELD2_HP_HYPERACTIVE * (bitCount[hp2 & black & shields & hyperactive]
                                       - bitCount[hp2 & white & shields & hyperactive])
           + SHIELD3_HP_FARAWAY * (bitCount[hp3 & black & shields & faraway]
                                   - bitCount[hp3 & white & shields & faraway])
           + SHIELD3_HP_INACTIVE * (bitCount[hp3 & black & shields & ~active & ~faraway]
                                    - bitCount[hp3 & white & shields & ~active & ~faraway])
           + SHIELD3_HP_ACTIVE * (bitCount[hp3 & black & shields & active & ~superactive]
                                  - bitCount[hp3 & white & shields & active & ~superactive])
           + SHIELD3_HP_SUPERACTIVE * (bitCount[hp3 & black & shields & superactive & ~hyperactive]
                                       - bitCount[hp3 & white & shields & superactive & ~hyperactive])
           + SHIELD3_HP_HYPERACTIVE * (bitCount[hp3 & black & shields & hyperactive]
                                       - bitCount[hp3 & white & shields & hyperactive])
           + SHIELD4_HP_FARAWAY * (bitCount[hp4 & black & shields & faraway]
                                   - bitCount[hp4 & white & shields & faraway])
           + SHIELD4_HP_INACTIVE * (bitCount[hp4 & black & shields & ~active & ~faraway]
                                    - bitCount[hp4 & white & shields & ~active & ~faraway])
           + SHIELD4_HP_ACTIVE * (bitCount[hp4 & black & shields & active & ~superactive]
                                  - bitCount[hp4 & white & shields & active & ~superactive])
           + SHIELD4_HP_SUPERACTIVE * (bitCount[hp4 & black & shields & superactive & ~hyperactive]
                                       - bitCount[hp4 & white & shields & superactive & ~hyperactive])
           + SHIELD4_HP_HYPERACTIVE * (bitCount[hp4 & black & shields & hyperactive]
                                       - bitCount[hp4 & white & shields & hyperactive]);
}

#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

float State::abBlackSwap(float alpha, float beta, byte depth) {
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return (float) endGameScore;
    if (depth == 0) return evaluate();
    for (State offspring: this->getOffsprings()) {
        float score = offspring.abBlackAct(alpha, beta, depth - 1);
        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}

float State::abBlackSwap(byte depth) { return abBlackSwap((float) INT_MIN, (float) INT_MAX, depth); }

float State::abBlackAct(float alpha, float beta, byte depth) {
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return (float) endGameScore;
    if (depth == 0) return evaluate();
    for (State offspring: this->getOffsprings()) {
        float score = offspring.abWhiteSwap(alpha, beta, depth - 1);
        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}

float State::abBlackAct(byte depth) { return abBlackAct((float) INT_MIN, (float) INT_MAX, depth); }

float State::abWhiteSwap(float alpha, float beta, byte depth) {
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return (float) endGameScore;
    if (depth == 0) return evaluate();
    for (State offspring: this->getOffsprings()) {
        float score = offspring.abWhiteAct(alpha, beta, depth - 1);
        if (score <= alpha) return alpha;
        if (score < beta) beta = score;
    }
    return beta;
}

float State::abWhiteSwap(byte depth) { return abWhiteSwap((float) INT_MIN, (float) INT_MAX, depth); }

float State::abWhiteAct(float alpha, float beta, byte depth) {
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return (float) endGameScore;
    if (depth == 0) return evaluate();
    for (State offspring: this->getOffsprings()) {
        float score = offspring.abBlackSwap(alpha, beta, depth - 1);
        if (score <= alpha) return alpha;
        if (score < beta) beta = score;
    }
    return beta;
}

float State::abWhiteAct(byte depth) { return abWhiteAct((float) INT_MIN, (float) INT_MAX, depth); }

#pragma region MiniMax

float State::mmBlackSwap(byte depth) {
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return (float) endGameScore;
    if (depth == 0) return evaluate();
    float max = INT_MIN;
    for (State offspring: this->getOffsprings()) {
        float score = offspring.mmBlackAct(depth - 1);
        if (score > max) max = score;
    }
    return max;
}

float State::mmBlackAct(byte depth) {
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return (float) endGameScore;
    if (depth == 0) return evaluate();
    float max = INT_MIN;
    for (State offspring: this->getOffsprings()) {
        float score = offspring.mmWhiteSwap(depth - 1);
        if (score > max) max = score;
    }
    return max;
}

float State::mmWhiteSwap(byte depth) {
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return (float) endGameScore;
    if (depth == 0) return evaluate();
    float min = INT_MAX;
    for (State offspring: this->getOffsprings()) {
        float score = offspring.mmWhiteAct(depth - 1);
        if (score < min) min = score;
    }
    return min;
}

float State::mmWhiteAct(byte depth) {
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return (float) endGameScore;
    if (depth == 0) return evaluate();
    float min = INT_MAX;
    for (State offspring: this->getOffsprings()) {
        float score = offspring.mmBlackSwap(depth - 1);
        if (score < min) min = score;
    }
    return min;
}

#pragma endregion

#pragma clang diagnostic pop

vector<State> State::getOffsprings() {
    vector<State> offsprings;
    ushort ours = (stateInfo & 0b1) == BLACK ? black : white;
    ushort theirs = (stateInfo & 0b1) == BLACK ? white : black;
    ushort swapTargets;
    ushort underAttack;
    ushort healTargets;
    ushort medic;
    byte medicIndex;
    ushort medicUp;
    ushort medicDown;
    ushort medicLeft;
    ushort medicRight;
    ushort wizardSwap;

    switch ((stateInfo >> 1) & 0b1) { // Phase
        case SWAP:
            swapTargets = (black | white) & ~(theirs & shields);
            for (auto &pair: switchPairs[ours & active]) {
                if ((1 << pair[1] & swapTargets) != 0) {
                    offsprings.push_back(this->swapPieces(pair[0], pair[1]));
                }
            }
            break;

        case ACT:

            byte skipMask = (stateInfo & 0b1) == BLACK ? 0b11100011 : 0b00011100;

            // Double attack
            if (active & ours & knights) {
                vector<array<byte, 2>> targets = knightTargetPairs[
                        compressedKnightTargets[
                                meleeTargets[knights & active & ours] & theirs
                        ][
                                compressedKnights[active & ours & knights]
                        ]
                ][compressedKnights[active & ours & knights]];
                for (auto &target: targets) {
                    offsprings.push_back(this->damage(1 << target[0], 1 << target[1], skipMask));
                }
            }

            // Heal
            medic = active & ours & medics;
            if (medic) {
                medicIndex = idx[medic];
                healTargets = ours & ((~kings & ~shields & ~hp3) | ((kings | shields) & ~hp4));
                medicUp = (medic << 4) & healTargets;
                medicDown = (medic >> 4) & healTargets;
                medicLeft = medicIndex % 4 != 0 ? (medic >> 1) & healTargets : 0;
                medicRight = medicIndex % 4 != 3 ? (medic << 1) & healTargets : 0;
                if (medicUp && medicDown && medicLeft && medicRight) {
                    offsprings.push_back(this->heal(medicUp, medicDown, medicLeft, medicRight, skipMask));
                }
                if (medicUp && medicDown && medicLeft) {
                    offsprings.push_back(this->heal(medicUp, medicDown, medicLeft, skipMask));
                }
                if (medicUp && medicDown && medicRight) {
                    offsprings.push_back(this->heal(medicUp, medicDown, medicRight, skipMask));
                }
                if (medicUp && medicLeft && medicRight) {
                    offsprings.push_back(this->heal(medicUp, medicLeft, medicRight, skipMask));
                }
                if (medicDown && medicLeft && medicRight) {
                    offsprings.push_back(this->heal(medicDown, medicLeft, medicRight, skipMask));
                }
                if (medicUp && medicDown) {
                    offsprings.push_back(this->heal(medicUp, medicDown, skipMask));
                }
                if (medicUp && medicLeft) {
                    offsprings.push_back(this->heal(medicUp, medicLeft, skipMask));
                }
                if (medicUp && medicRight) {
                    offsprings.push_back(this->heal(medicUp, medicRight, skipMask));
                }
                if (medicDown && medicLeft) {
                    offsprings.push_back(this->heal(medicDown, medicLeft, skipMask));
                }
                if (medicDown && medicRight) {
                    offsprings.push_back(this->heal(medicDown, medicRight, skipMask));
                }
                if (medicLeft && medicRight) {
                    offsprings.push_back(this->heal(medicLeft, medicRight, skipMask));
                }
                if (medicUp) {
                    offsprings.push_back(this->heal(medicUp, skipMask));
                }
                if (medicDown) {
                    offsprings.push_back(this->heal(medicDown, skipMask));
                }
                if (medicLeft) {
                    offsprings.push_back(this->heal(medicLeft, skipMask));
                }
                if (medicRight) {
                    offsprings.push_back(this->heal(medicRight, skipMask));
                }
            }

            // Single attack
            underAttack = (meleeTargets[(kings | knights | archers) & active & ours] |
                           archerTargets[archers & active & ours][idx[shields & theirs]]) & theirs;
            for (byte i = 0; underAttack != 0; underAttack >>= 1) {
                if (underAttack & 1) {
                    offsprings.push_back(this->damage(1 << i, skipMask));
                }
                i++;
            }

            // Teleport
            if (active & ours & wizards) {
                wizardSwap = ours & ~wizards;
                for (byte i = 0; wizardSwap != 0; wizardSwap >>= 1) {
                    if (wizardSwap & 1) {
                        offsprings.push_back(this->wizardSwap(idx[ours & wizards], i, skipMask));
                    }
                    i++;
                }
            }

            // Skip
            if ((stateInfo & 0b1) == BLACK) {
                if (!(stateInfo & (1 << 2))) {
                    offsprings.push_back(
                            State(black, white, hp1, hp2, hp3, hp4, kings, knights, archers, medics, wizards, shields,
                                  stateInfo ^ 0b11 | 0b00000100));
                } else if (!(stateInfo & (1 << 3))) {
                    offsprings.push_back(
                            State(black, white, hp1, hp2, hp3, hp4, kings, knights, archers, medics, wizards, shields,
                                  stateInfo ^ 0b11 | 0b00001000));
                } else {
                    offsprings.push_back(
                            State(black, white, hp1, hp2, hp3, hp4, kings, knights, archers, medics, wizards, shields,
                                  stateInfo ^ 0b11 | 0b00010000));
                }
            } else {
                if (!(stateInfo & (1 << 5))) {
                    offsprings.push_back(
                            State(black, white, hp1, hp2, hp3, hp4, kings, knights, archers, medics, wizards, shields,
                                  stateInfo ^ 0b11 | 0b00100000));
                } else if (!(stateInfo & (1 << 6))) {
                    offsprings.push_back(
                            State(black, white, hp1, hp2, hp3, hp4, kings, knights, archers, medics, wizards, shields,
                                  stateInfo ^ 0b11 | 0b01000000));
                } else {
                    offsprings.push_back(
                            State(black, white, hp1, hp2, hp3, hp4, kings, knights, archers, medics, wizards, shields,
                                  stateInfo ^ 0b11 | 0b10000000));
                }
            }

            break;
    }
    return offsprings;
}

State State::swapPieces(byte piece1, byte piece2) const {
    return State(
            (((((black >> piece1) & 1) ^ ((black >> piece2) & 1)) << piece1) |
             ((((black >> piece1) & 1) ^ ((black >> piece2) & 1)) << piece2)) ^ black,
            (((((white >> piece1) & 1) ^ ((white >> piece2) & 1)) << piece1) |
             ((((white >> piece1) & 1) ^ ((white >> piece2) & 1)) << piece2)) ^ white,
            (((((hp1 >> piece1) & 1) ^ ((hp1 >> piece2) & 1)) << piece1) |
             ((((hp1 >> piece1) & 1) ^ ((hp1 >> piece2) & 1)) << piece2)) ^ hp1,
            (((((hp2 >> piece1) & 1) ^ ((hp2 >> piece2) & 1)) << piece1) |
             ((((hp2 >> piece1) & 1) ^ ((hp2 >> piece2) & 1)) << piece2)) ^ hp2,
            (((((hp3 >> piece1) & 1) ^ ((hp3 >> piece2) & 1)) << piece1) |
             ((((hp3 >> piece1) & 1) ^ ((hp3 >> piece2) & 1)) << piece2)) ^ hp3,
            (((((hp4 >> piece1) & 1) ^ ((hp4 >> piece2) & 1)) << piece1) |
             ((((hp4 >> piece1) & 1) ^ ((hp4 >> piece2) & 1)) << piece2)) ^ hp4,
            (((((kings >> piece1) & 1) ^ ((kings >> piece2) & 1)) << piece1) |
             ((((kings >> piece1) & 1) ^ ((kings >> piece2) & 1)) << piece2)) ^ kings,
            (((((knights >> piece1) & 1) ^ ((knights >> piece2) & 1)) << piece1) |
             ((((knights >> piece1) & 1) ^ ((knights >> piece2) & 1)) << piece2)) ^ knights,
            (((((archers >> piece1) & 1) ^ ((archers >> piece2) & 1)) << piece1) |
             ((((archers >> piece1) & 1) ^ ((archers >> piece2) & 1)) << piece2)) ^ archers,
            (((((medics >> piece1) & 1) ^ ((medics >> piece2) & 1)) << piece1) |
             ((((medics >> piece1) & 1) ^ ((medics >> piece2) & 1)) << piece2)) ^ medics,
            (((((wizards >> piece1) & 1) ^ ((wizards >> piece2) & 1)) << piece1) |
             ((((wizards >> piece1) & 1) ^ ((wizards >> piece2) & 1)) << piece2)) ^ wizards,
            (((((shields >> piece1) & 1) ^ ((shields >> piece2) & 1)) << piece1) |
             ((((shields >> piece1) & 1) ^ ((shields >> piece2) & 1)) << piece2)) ^ shields,
            stateInfo ^ 0b10
    );
}

State State::damage(ushort piece, byte skipMask) const {
    return State(
            black & (black ^ (piece & hp1)),
            white & (white ^ (piece & hp1)),
            (hp1 & (hp1 ^ piece)) | (piece & hp2),
            (hp2 & (hp2 ^ piece)) | (piece & hp3),
            (hp3 & (hp3 ^ piece)) | (piece & hp4),
            hp4 & (hp4 ^ piece),
            kings & (kings ^ (piece & hp1)),
            knights & (knights ^ (piece & hp1)),
            archers & (archers ^ (piece & hp1)),
            medics & (medics ^ (piece & hp1)),
            wizards & (wizards ^ (piece & hp1)),
            shields & (shields ^ (piece & hp1)),
            (stateInfo ^ 0b11) & skipMask
    );
}

State State::damage(ushort piece1, ushort piece2, byte skipMask) const {
    return State(
            black & (black ^ ((piece1 | piece2) & hp1)),
            white & (white ^ ((piece1 | piece2) & hp1)),
            (hp1 & (hp1 ^ (piece1 | piece2))) | ((piece1 | piece2) & hp2),
            (hp2 & (hp2 ^ (piece1 | piece2))) | ((piece1 | piece2) & hp3),
            (hp3 & (hp3 ^ (piece1 | piece2))) | ((piece1 | piece2) & hp4),
            hp4 & (hp4 ^ (piece1 | piece2)),
            kings & (kings ^ ((piece1 | piece2) & hp1)),
            knights & (knights ^ ((piece1 | piece2) & hp1)),
            archers & (archers ^ ((piece1 | piece2) & hp1)),
            medics & (medics ^ ((piece1 | piece2) & hp1)),
            wizards & (wizards ^ ((piece1 | piece2) & hp1)),
            shields & (shields ^ ((piece1 | piece2) & hp1)),
            (stateInfo ^ 0b11) & skipMask
    );
}

State State::heal(ushort piece, byte skipMask) const {
    return State(
            black,
            white,
            hp1 & ~piece,
            hp2 & ~(hp2 & piece) | (hp1 & piece),
            hp3 & ~(hp3 & piece) | (hp2 & piece),
            hp4 | (hp3 & piece),
            kings,
            knights,
            archers,
            medics,
            wizards,
            shields,
            (stateInfo ^ 0b11) & skipMask
    );
}

State State::heal(ushort piece1, ushort piece2, byte skipMask) const {
    return State(
            black,
            white,
            hp1 & ~piece1 & ~piece2,
            hp2 & ~(hp2 & (piece1 | piece2)) | (hp1 & (piece1 | piece2)),
            hp3 & ~(hp3 & (piece1 | piece2)) | (hp2 & (piece1 | piece2)),
            hp4 | (hp3 & (piece1 | piece2)),
            kings,
            knights,
            archers,
            medics,
            wizards,
            shields,
            (stateInfo ^ 0b11) & skipMask
    );
}

State State::heal(ushort piece1, ushort piece2, ushort piece3, byte skipMask) const {
    return State(
            black,
            white,
            hp1 & ~piece1 & ~piece2 & ~piece3,
            hp2 & ~(hp2 & (piece1 | piece2 | piece3)) | (hp1 & (piece1 | piece2 | piece3)),
            hp3 & ~(hp3 & (piece1 | piece2 | piece3)) | (hp2 & (piece1 | piece2 | piece3)),
            hp4 | (hp3 & (piece1 | piece2 | piece3)),
            kings,
            knights,
            archers,
            medics,
            wizards,
            shields,
            (stateInfo ^ 0b11) & skipMask
    );
}

State State::heal(ushort piece1, ushort piece2, ushort piece3, ushort piece4, byte skipMask) const {
    return State(
            black,
            white,
            hp1 & ~piece1 & ~piece2 & ~piece3 & ~piece4,
            hp2 & ~(hp2 & (piece1 | piece2 | piece3 | piece4)) | (hp1 & (piece1 | piece2 | piece3 | piece4)),
            hp3 & ~(hp3 & (piece1 | piece2 | piece3 | piece4)) | (hp2 & (piece1 | piece2 | piece3 | piece4)),
            hp4 | (hp3 & (piece1 | piece2 | piece3 | piece4)),
            kings,
            knights,
            archers,
            medics,
            wizards,
            shields,
            (stateInfo ^ 0b11) & skipMask
    );
}

State State::wizardSwap(byte piece1, byte piece2, byte skipMask) const {
    return State(
            (((((black >> piece1) & 1) ^ ((black >> piece2) & 1)) << piece1) |
             ((((black >> piece1) & 1) ^ ((black >> piece2) & 1)) << piece2)) ^ black,
            (((((white >> piece1) & 1) ^ ((white >> piece2) & 1)) << piece1) |
             ((((white >> piece1) & 1) ^ ((white >> piece2) & 1)) << piece2)) ^ white,
            (((((hp1 >> piece1) & 1) ^ ((hp1 >> piece2) & 1)) << piece1) |
             ((((hp1 >> piece1) & 1) ^ ((hp1 >> piece2) & 1)) << piece2)) ^ hp1,
            (((((hp2 >> piece1) & 1) ^ ((hp2 >> piece2) & 1)) << piece1) |
             ((((hp2 >> piece1) & 1) ^ ((hp2 >> piece2) & 1)) << piece2)) ^ hp2,
            (((((hp3 >> piece1) & 1) ^ ((hp3 >> piece2) & 1)) << piece1) |
             ((((hp3 >> piece1) & 1) ^ ((hp3 >> piece2) & 1)) << piece2)) ^ hp3,
            (((((hp4 >> piece1) & 1) ^ ((hp4 >> piece2) & 1)) << piece1) |
             ((((hp4 >> piece1) & 1) ^ ((hp4 >> piece2) & 1)) << piece2)) ^ hp4,
            (((((kings >> piece1) & 1) ^ ((kings >> piece2) & 1)) << piece1) |
             ((((kings >> piece1) & 1) ^ ((kings >> piece2) & 1)) << piece2)) ^ kings,
            (((((knights >> piece1) & 1) ^ ((knights >> piece2) & 1)) << piece1) |
             ((((knights >> piece1) & 1) ^ ((knights >> piece2) & 1)) << piece2)) ^ knights,
            (((((archers >> piece1) & 1) ^ ((archers >> piece2) & 1)) << piece1) |
             ((((archers >> piece1) & 1) ^ ((archers >> piece2) & 1)) << piece2)) ^ archers,
            (((((medics >> piece1) & 1) ^ ((medics >> piece2) & 1)) << piece1) |
             ((((medics >> piece1) & 1) ^ ((medics >> piece2) & 1)) << piece2)) ^ medics,
            (((((wizards >> piece1) & 1) ^ ((wizards >> piece2) & 1)) << piece1) |
             ((((wizards >> piece1) & 1) ^ ((wizards >> piece2) & 1)) << piece2)) ^ wizards,
            (((((shields >> piece1) & 1) ^ ((shields >> piece2) & 1)) << piece1) |
             ((((shields >> piece1) & 1) ^ ((shields >> piece2) & 1)) << piece2)) ^ shields,
            (stateInfo ^ 0b11) & skipMask
    );
}
