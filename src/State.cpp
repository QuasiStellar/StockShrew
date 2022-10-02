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

static byte bitCount[65536];

static vector<array<byte, 2>> switchPairs[65536];

static ushort meleeTargets[65536];

static ushort archerTargets[65536][17];

static byte compressedKnights[65536];

static byte compressedKnightTargets[65536][256];

static vector<array<byte, 2>> knightTargetPairs[256][256];

static byte index[65536];

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

    ushort blackUp = black << 4;
    ushort blackDown = black >> 4;
    ushort blackRight = (black << 1) & NOT_RIGHTMOST;
    ushort blackLeft = (black >> 1) & NOT_LEFTMOST;

    ushort whiteUp = white << 4;
    ushort whiteDown = white >> 4;
    ushort whiteRight = (white << 1) & NOT_RIGHTMOST;
    ushort whiteLeft = (white >> 1) & NOT_LEFTMOST;

    active = (black & (blackUp
                       | blackDown
                       | blackRight
                       | blackLeft))
             | (white & (whiteUp
                         | whiteDown
                         | whiteRight
                         | whiteLeft));

    superactive = (black & ((blackUp & blackDown)
                            | (blackRight & blackLeft)
                            | (blackUp & blackRight)
                            | (blackRight & blackDown)
                            | (blackDown & blackLeft)
                            | (blackLeft & blackUp)))
                  | (white & ((whiteUp & whiteDown)
                              | (whiteRight & whiteLeft)
                              | (whiteUp & whiteRight)
                              | (whiteRight & whiteDown)
                              | (whiteDown & whiteLeft)
                              | (whiteLeft & whiteUp)));

    hyperactive = (black & ((blackUp & blackRight & blackLeft)
                            | (blackDown & blackRight & blackLeft)
                            | (blackRight & blackUp & blackDown)
                            | (blackLeft & blackUp & blackDown)))
                  | (white & ((whiteUp & whiteRight & whiteLeft)
                              | (whiteDown & whiteRight & whiteLeft)
                              | (whiteRight & whiteUp & whiteDown)
                              | (whiteLeft & whiteUp & whiteDown)));

    ushort blackActive = black & active;
    ushort whiteActive = white & active;

    faraway = (black & ~((blackActive << 4)
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
              | (white & ~((whiteActive << 4)
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

    ushort blackUp = this->black << 4;
    ushort blackDown = this->black >> 4;
    ushort blackRight = (this->black << 1) & NOT_RIGHTMOST;
    ushort blackLeft = (this->black >> 1) & NOT_LEFTMOST;

    ushort whiteUp = this->white << 4;
    ushort whiteDown = this->white >> 4;
    ushort whiteRight = (this->white << 1) & NOT_RIGHTMOST;
    ushort whiteLeft = (this->white >> 1) & NOT_LEFTMOST;

    active = (this->black & (blackUp
                             | blackDown
                             | blackRight
                             | blackLeft))
             | (this->white & (whiteUp
                               | whiteDown
                               | whiteRight
                               | whiteLeft));

    superactive = (this->black & ((blackUp & blackDown)
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

    hyperactive = (this->black & ((blackUp & blackRight & blackLeft)
                                  | (blackDown & blackRight & blackLeft)
                                  | (blackRight & blackUp & blackDown)
                                  | (blackLeft & blackUp & blackDown)))
                  | (this->white & ((whiteUp & whiteRight & whiteLeft)
                                    | (whiteDown & whiteRight & whiteLeft)
                                    | (whiteRight & whiteUp & whiteDown)
                                    | (whiteLeft & whiteUp & whiteDown)));

    ushort blackActive = this->black & active;
    ushort whiteActive = this->white & active;

    faraway = (this->black & ~((blackActive << 4)
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
}

float State::search(byte depth) {
    setBitCount();
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

#pragma region calibration

const float SWAP_ADVANTAGE = 3.f;
const float KING1_HP_FARAWAY = 1.f;
const float KING1_HP_INACTIVE = 2.2f;
const float KING1_HP_ACTIVE = 3.2f;
const float KING1_HP_SUPERACTIVE = 4.2f;
const float KING1_HP_HYPERACTIVE = 4.7f;
const float KING2_HP_FARAWAY = 2.f;
const float KING2_HP_INACTIVE = 3.2f;
const float KING2_HP_ACTIVE = 4.2f;
const float KING2_HP_SUPERACTIVE = 5.2f;
const float KING2_HP_HYPERACTIVE = 5.7f;
const float KING3_HP_FARAWAY = 3.f;
const float KING3_HP_INACTIVE = 4.2f;
const float KING3_HP_ACTIVE = 5.2f;
const float KING3_HP_SUPERACTIVE = 6.2f;
const float KING3_HP_HYPERACTIVE = 6.7f;
const float KING4_HP_FARAWAY = 4.f;
const float KING4_HP_INACTIVE = 5.2f;
const float KING4_HP_ACTIVE = 6.2f;
const float KING4_HP_SUPERACTIVE = 7.2f;
const float KING4_HP_HYPERACTIVE = 7.7f;
const float KNIGHT1_HP_FARAWAY = 1.6f;
const float KNIGHT1_HP_INACTIVE = 2.8f;
const float KNIGHT1_HP_ACTIVE = 3.8f;
const float KNIGHT1_HP_SUPERACTIVE = 4.8f;
const float KNIGHT1_HP_HYPERACTIVE = 5.3f;
const float KNIGHT2_HP_FARAWAY = 2.6f;
const float KNIGHT2_HP_INACTIVE = 3.8f;
const float KNIGHT2_HP_ACTIVE = 4.8f;
const float KNIGHT2_HP_SUPERACTIVE = 5.8f;
const float KNIGHT2_HP_HYPERACTIVE = 6.3f;
const float KNIGHT3_HP_FARAWAY = 3.6f;
const float KNIGHT3_HP_INACTIVE = 4.8f;
const float KNIGHT3_HP_ACTIVE = 5.8f;
const float KNIGHT3_HP_SUPERACTIVE = 6.8f;
const float KNIGHT3_HP_HYPERACTIVE = 7.3f;
const float ARCHER1_HP_FARAWAY = 1.2f;
const float ARCHER1_HP_INACTIVE = 2.4f;
const float ARCHER1_HP_ACTIVE = 3.4f;
const float ARCHER1_HP_SUPERACTIVE = 4.4f;
const float ARCHER1_HP_HYPERACTIVE = 4.9f;
const float ARCHER2_HP_FARAWAY = 2.2f;
const float ARCHER2_HP_INACTIVE = 3.4f;
const float ARCHER2_HP_ACTIVE = 4.4f;
const float ARCHER2_HP_SUPERACTIVE = 5.4f;
const float ARCHER2_HP_HYPERACTIVE = 5.9f;
const float ARCHER3_HP_FARAWAY = 3.2f;
const float ARCHER3_HP_INACTIVE = 4.4f;
const float ARCHER3_HP_ACTIVE = 5.4f;
const float ARCHER3_HP_SUPERACTIVE = 6.4f;
const float ARCHER3_HP_HYPERACTIVE = 6.9f;
const float MEDIC1_HP_FARAWAY = 1.f;
const float MEDIC1_HP_INACTIVE = 2.5f;
const float MEDIC1_HP_ACTIVE = 3.5f;
const float MEDIC1_HP_SUPERACTIVE = 4.5f;
const float MEDIC1_HP_HYPERACTIVE = 5.f;
const float MEDIC2_HP_FARAWAY = 2.f;
const float MEDIC2_HP_INACTIVE = 3.5f;
const float MEDIC2_HP_ACTIVE = 4.5f;
const float MEDIC2_HP_SUPERACTIVE = 5.5f;
const float MEDIC2_HP_HYPERACTIVE = 6.f;
const float MEDIC3_HP_FARAWAY = 3.f;
const float MEDIC3_HP_INACTIVE = 4.5f;
const float MEDIC3_HP_ACTIVE = 5.5f;
const float MEDIC3_HP_SUPERACTIVE = 6.5f;
const float MEDIC3_HP_HYPERACTIVE = 7.f;
const float WIZARD1_HP_FARAWAY = 0.8f;
const float WIZARD1_HP_INACTIVE = 2.f;
const float WIZARD1_HP_ACTIVE = 3.f;
const float WIZARD1_HP_SUPERACTIVE = 4.f;
const float WIZARD1_HP_HYPERACTIVE = 4.5f;
const float WIZARD2_HP_FARAWAY = 1.8f;
const float WIZARD2_HP_INACTIVE = 3.f;
const float WIZARD2_HP_ACTIVE = 4.f;
const float WIZARD2_HP_SUPERACTIVE = 5.f;
const float WIZARD2_HP_HYPERACTIVE = 5.5f;
const float WIZARD3_HP_FARAWAY = 2.8f;
const float WIZARD3_HP_INACTIVE = 4.f;
const float WIZARD3_HP_ACTIVE = 5.f;
const float WIZARD3_HP_SUPERACTIVE = 6.f;
const float WIZARD3_HP_HYPERACTIVE = 6.5f;
const float SHIELD1_HP_FARAWAY = 1.4f;
const float SHIELD1_HP_INACTIVE = 2.6f;
const float SHIELD1_HP_ACTIVE = 3.6f;
const float SHIELD1_HP_SUPERACTIVE = 4.6f;
const float SHIELD1_HP_HYPERACTIVE = 5.1f;
const float SHIELD2_HP_FARAWAY = 2.4f;
const float SHIELD2_HP_INACTIVE = 3.6f;
const float SHIELD2_HP_ACTIVE = 4.6f;
const float SHIELD2_HP_SUPERACTIVE = 5.6f;
const float SHIELD2_HP_HYPERACTIVE = 6.1f;
const float SHIELD3_HP_FARAWAY = 3.4f;
const float SHIELD3_HP_INACTIVE = 4.6f;
const float SHIELD3_HP_ACTIVE = 5.6f;
const float SHIELD3_HP_SUPERACTIVE = 6.6f;
const float SHIELD3_HP_HYPERACTIVE = 7.1f;
const float SHIELD4_HP_FARAWAY = 4.4f;
const float SHIELD4_HP_INACTIVE = 5.6f;
const float SHIELD4_HP_ACTIVE = 6.6f;
const float SHIELD4_HP_SUPERACTIVE = 7.6f;
const float SHIELD4_HP_HYPERACTIVE = 8.1f;

#pragma endregion

#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-narrowing-conversions"

float State::evaluate() const {
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
                    offsprings.push_back(this->damage(1 << target[0], 1 << target[1]));
                }
            }

            // Heal
            medic = active & ours & medics;
            if (medic) {
                medicIndex = index[medic];
                healTargets = ours & ((~kings & ~shields & ~hp3) | ((kings | shields) & ~hp4));
                medicUp = (medic << 4) & healTargets;
                medicDown = (medic >> 4) & healTargets;
                medicLeft = medicIndex % 4 != 0 ? (medic >> 1) & healTargets : 0;
                medicRight = medicIndex % 4 != 3 ? (medic << 1) & healTargets : 0;
                if (medicUp && medicDown && medicLeft && medicRight) {
                    offsprings.push_back(this->heal(medicUp, medicDown, medicLeft, medicRight));
                }
                if (medicUp && medicDown && medicLeft) {
                    offsprings.push_back(this->heal(medicUp, medicDown, medicLeft));
                }
                if (medicUp && medicDown && medicRight) {
                    offsprings.push_back(this->heal(medicUp, medicDown, medicRight));
                }
                if (medicUp && medicLeft && medicRight) {
                    offsprings.push_back(this->heal(medicUp, medicLeft, medicRight));
                }
                if (medicDown && medicLeft && medicRight) {
                    offsprings.push_back(this->heal(medicDown, medicLeft, medicRight));
                }
                if (medicUp && medicDown) {
                    offsprings.push_back(this->heal(medicUp, medicDown));
                }
                if (medicUp && medicLeft) {
                    offsprings.push_back(this->heal(medicUp, medicLeft));
                }
                if (medicUp && medicRight) {
                    offsprings.push_back(this->heal(medicUp, medicRight));
                }
                if (medicDown && medicLeft) {
                    offsprings.push_back(this->heal(medicDown, medicLeft));
                }
                if (medicDown && medicRight) {
                    offsprings.push_back(this->heal(medicDown, medicRight));
                }
                if (medicLeft && medicRight) {
                    offsprings.push_back(this->heal(medicLeft, medicRight));
                }
                if (medicUp) {
                    offsprings.push_back(this->heal(medicUp));
                }
                if (medicDown) {
                    offsprings.push_back(this->heal(medicDown));
                }
                if (medicLeft) {
                    offsprings.push_back(this->heal(medicLeft));
                }
                if (medicRight) {
                    offsprings.push_back(this->heal(medicRight));
                }
            }

            // Single attack
            underAttack = (meleeTargets[(kings | knights | archers) & active & ours] |
                           archerTargets[archers & active & ours][index[shields & theirs]]) & theirs;
            for (byte i = 0; underAttack != 0; underAttack >>= 1) {
                if (underAttack & 1) {
                    offsprings.push_back(this->damage(1 << i));
                }
                i++;
            }

            // Teleport
            if (active & ours & wizards) {
                wizardSwap = ours & ~wizards;
                for (byte i = 0; wizardSwap != 0; wizardSwap >>= 1) {
                    if (wizardSwap & 1) {
                        offsprings.push_back(this->swapPieces(index[ours & wizards], i));
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

State State::damage(ushort piece) const {
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
            stateInfo ^ 0b11
    );
}

State State::damage(ushort piece1, ushort piece2) const {
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
            stateInfo ^ 0b11
    );
}

State State::heal(ushort piece) const {
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
            stateInfo ^ 0b11
    );
}

State State::heal(ushort piece1, ushort piece2) const {
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
            stateInfo ^ 0b11
    );
}

State State::heal(ushort piece1, ushort piece2, ushort piece3) const {
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
            stateInfo ^ 0b11
    );
}

State State::heal(ushort piece1, ushort piece2, ushort piece3, ushort piece4) const {
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
            stateInfo ^ 0b11
    );
}

static bool setup = false;

void State::setUp() {
    if (setup) {
        cout << "Lookup tables already set up." << endl;
        return;
    }
    cout << "Setting up lookup tables..." << endl;
    setBitCount();
    setSwitchPairs();
    setMeleeTargets();
    setArcherTargets();
    setCompressedKnights();
    setCompressedKnightTargets();
    setKnightTargetPairs();
    setIndex();
    cout << "Lookup tables set up." << endl;
    setup = true;
}

void State::setBitCount() {
    bitCount[0] = 0;
    for (int i = 0; i < 65536; i++) {
        bitCount[i] = (i & 1) + bitCount[i / 2];
    }
}

void State::setSwitchPairs() {
    for (int i = 0; i < 65536; i++) {
        switchPairs[i] = vector<array<byte, 2>>();
        for (int j = 0; j < 16; j++) {
            if ((1 << j) & i) {
                if (j + 4 < 16) {
                    switchPairs[i].push_back(array<byte, 2>{(byte) j, (byte) (j + 4)});
                }
                if (!(1 << (j - 4) & i) && j - 4 > -1) {
                    switchPairs[i].push_back(array<byte, 2>{(byte) j, (byte) (j - 4)});
                }
                if (j % 4 != 3) {
                    switchPairs[i].push_back(array<byte, 2>{(byte) j, (byte) (j + 1)});
                }
                if (!(1 << (j - 1) & i) && j % 4 != 0) {
                    switchPairs[i].push_back(array<byte, 2>{(byte) j, (byte) (j - 1)});
                }
            }
        }
    }
}

void State::setMeleeTargets() {
    for (int i = 0; i < 65536; i++) {
        ushort map = 0;
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

void State::setArcherTargets() {
    for (int i = 0; i < 65536; i++) {
        for (int shield = 0; shield < 17; shield++) {
            ushort map = 0;
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

void State::setCompressedKnights() {
    for (int i = 0; i < 16; i++) {
        compressedKnights[1 << i] = i * 16 + i;
    }
    for (int i = 0; i < 16; i++) {
        for (int j = i + 1; i < 16; i++) {
            compressedKnights[(1 << i) + (1 << j)] = i * 16 + j;
        }
    }
}

void State::setCompressedKnightTargets() {
    for (int i = 0; i < 65536; i++) {
        for (int j = 0; j < 256; j++) {
            byte firstKnight = j % 16;
            byte secondKnight = j / 16;
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

void State::setKnightTargetPairs() {
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            knightTargetPairs[i][j] = vector<array<byte, 2>>();
            byte firstKnight = j % 16;
            byte secondKnight = j / 16;
            if (i & (1 << 7) && i & (1 << 6)) {
                knightTargetPairs[i][j].push_back(array<byte, 2>{(byte) (firstKnight + 4), (byte) (firstKnight - 4)});
            }
            if (i & (1 << 7) && i & (1 << 5)) {
                knightTargetPairs[i][j].push_back(array<byte, 2>{(byte) (firstKnight + 4), (byte) (firstKnight - 1)});
            }
            if (i & (1 << 7) && i & (1 << 4) && secondKnight - firstKnight != 5) {
                knightTargetPairs[i][j].push_back(array<byte, 2>{(byte) (firstKnight + 4), (byte) (firstKnight + 1)});
            }
            if (i & (1 << 6) && i & (1 << 5)) {
                knightTargetPairs[i][j].push_back(array<byte, 2>{(byte) (firstKnight - 4), (byte) (firstKnight - 1)});
            }
            if (i & (1 << 6) && i & (1 << 4) && firstKnight - secondKnight != 3) {
                knightTargetPairs[i][j].push_back(array<byte, 2>{(byte) (firstKnight - 4), (byte) (firstKnight + 1)});
            }
            if (i & (1 << 5) && i & (1 << 4)) {
                knightTargetPairs[i][j].push_back(array<byte, 2>{(byte) (firstKnight - 1), (byte) (firstKnight + 1)});
            }
            if (i & (1 << 3) && i & (1 << 2)) {
                knightTargetPairs[i][j].push_back(array<byte, 2>{(byte) (secondKnight + 4), (byte) (secondKnight - 4)});
            }
            if (i & (1 << 3) && i & (1 << 1)) {
                knightTargetPairs[i][j].push_back(array<byte, 2>{(byte) (secondKnight + 4), (byte) (secondKnight - 1)});
            }
            if (i & (1 << 3) && i & (1 << 0)) {
                knightTargetPairs[i][j].push_back(array<byte, 2>{(byte) (secondKnight + 4), (byte) (secondKnight + 1)});
            }
            if (i & (1 << 2) && i & (1 << 1)) {
                knightTargetPairs[i][j].push_back(array<byte, 2>{(byte) (secondKnight - 4), (byte) (secondKnight - 1)});
            }
            if (i & (1 << 2) && i & (1 << 0)) {
                knightTargetPairs[i][j].push_back(array<byte, 2>{(byte) (secondKnight - 4), (byte) (secondKnight + 1)});
            }
            if (i & (1 << 1) && i & (1 << 0)) {
                knightTargetPairs[i][j].push_back(array<byte, 2>{(byte) (secondKnight - 1), (byte) (secondKnight + 1)});
            }
        }
    }
}

void State::setIndex() {
    for (int i = 1; i < 65536; i++) {
        if ((i & (i - 1)) != 0) continue;
        int j = i;
        for (; j != 1; index[i]++) j >>= 1;
    }
}
