#include "State.h"

const uint8 BLACK = 0b0;
const uint8 WHITE = 0b1;
const uint8 SWAP = 0b0;
const uint8 ACT = 0b1;

const uint16 NOT_LEFTMOST = 0b0111011101110111;
const uint16 NOT_RIGHTMOST = 0b1110111011101110;
const uint16 NOT_LEFT_HALF = 0b0011001100110011;
const uint16 NOT_RIGHT_HALF = 0b1100110011001100;

long long State::stateCount;

unordered_set<StateRecord> State::history;

State::State(Piece *board[4][4], Side side, Phase phase, uint8 blackSkips, uint8 whiteSkips) {

    black = white = hp1 = hp2 = hp3 = hp4 = kings = knights = archers = medics = wizards = shields = 0;

    for (uint8 i = 0; i < 4; i++) {
        for (uint8 j = 0; j < 4; j++) {
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

    stateInfo = (int) side + ((int) phase << 1)
                + (blackSkips == 1 ? 0b0100 : 0)
                + (blackSkips == 2 ? 0b1000 : 0)
                + (blackSkips == 3 ? 0b1100 : 0)
                + (whiteSkips == 1 ? 0b010000 : 0)
                + (whiteSkips == 2 ? 0b100000 : 0)
                + (whiteSkips == 3 ? 0b110000 : 0);

    active = ((black & (black << 4 & (hp1 << 4 | hp2 << 4 | hp3 << 4 | hp4 << 4)
                        | black >> 4 & (hp1 >> 4 | hp2 >> 4 | hp3 >> 4 | hp4 >> 4)
                        | (black << 1) & NOT_RIGHTMOST & (hp1 << 1 | hp2 << 1 | hp3 << 1 | hp4 << 1)
                        | (black >> 1) & NOT_LEFTMOST & (hp1 >> 1 | hp2 >> 1 | hp3 >> 1 | hp4 >> 1)))
              | (white & (white << 4 & (hp1 << 4 | hp2 << 4 | hp3 << 4 | hp4 << 4)
                          | white >> 4 & (hp1 >> 4 | hp2 >> 4 | hp3 >> 4 | hp4 >> 4)
                          | (white << 1) & NOT_RIGHTMOST & (hp1 << 1 | hp2 << 1 | hp3 << 1 | hp4 << 1)
                          | (white >> 1) & NOT_LEFTMOST & (hp1 >> 1 | hp2 >> 1 | hp3 >> 1 | hp4 >> 1))))
             & (hp1 | hp2 | hp3 | hp4);
}

State::State(uint16 black, uint16 white,
             uint16 hp1, uint16 hp2, uint16 hp3, uint16 hp4,
             uint16 kings, uint16 knights, uint16 archers, uint16 medics, uint16 wizards, uint16 shields,
             uint8 stateInfo)
        : black(black),
          white(white),
          hp1(hp1),
          hp2(hp2),
          hp3(hp3),
          hp4(hp4),
          kings(kings),
          knights(knights),
          archers(archers),
          medics(medics),
          wizards(wizards),
          shields(shields),
          stateInfo(stateInfo) {

    active = ((black & (black << 4 & (hp1 << 4 | hp2 << 4 | hp3 << 4 | hp4 << 4)
                        | black >> 4 & (hp1 >> 4 | hp2 >> 4 | hp3 >> 4 | hp4 >> 4)
                        | (black << 1) & NOT_RIGHTMOST & (hp1 << 1 | hp2 << 1 | hp3 << 1 | hp4 << 1)
                        | (black >> 1) & NOT_LEFTMOST & (hp1 >> 1 | hp2 >> 1 | hp3 >> 1 | hp4 >> 1)))
              | (white & (white << 4 & (hp1 << 4 | hp2 << 4 | hp3 << 4 | hp4 << 4)
                          | white >> 4 & (hp1 >> 4 | hp2 >> 4 | hp3 >> 4 | hp4 >> 4)
                          | (white << 1) & NOT_RIGHTMOST & (hp1 << 1 | hp2 << 1 | hp3 << 1 | hp4 << 1)
                          | (white >> 1) & NOT_LEFTMOST & (hp1 >> 1 | hp2 >> 1 | hp3 >> 1 | hp4 >> 1))))
             & (hp1 | hp2 | hp3 | hp4);
}

float State::search(uint8 depth, Strategy strategy) {
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

pair<float, string> State::searchForMove(uint8 depth) {
    switch (stateInfo & 0b1) {
        case BLACK:
            switch ((stateInfo >> 1) & 0b1) {
                case SWAP:
                    return abBlackSwapBest(depth);
                case ACT:
                    return abBlackActBest(depth);
            }
        case WHITE:
            switch ((stateInfo >> 1) & 0b1) {
                case SWAP:
                    return abWhiteSwapBest(depth);
                case ACT:
                    return abWhiteActBest(depth);
            }
    }
    return pair(0.f, "");
}

int State::endGameScore() const {
    if ((stateInfo >> 2) & 11) return INT_MIN; // black is out of skips
    if ((stateInfo >> 4) & 11) return INT_MAX; // white is out of skips
    bool blackKing = (black & kings & (hp1 | hp2 | hp3 | hp4)) != 0;
    bool whiteKing = (white & kings & (hp1 | hp2 | hp3 | hp4)) != 0;
    bool blackActive = (black & active) != 0;
    bool whiteActive = (white & active) != 0;
    if (!blackActive && !whiteActive) return 0;
    if (!blackKing || !blackActive) return INT_MIN;
    if (!whiteKing || !whiteActive) return INT_MAX;
    if ((black & (white << 4 & (hp1 << 4 | hp2 << 4 | hp3 << 4 | hp4 << 4)
                  | white >> 4 & (hp1 >> 4 | hp2 >> 4 | hp3 >> 4 | hp4 >> 4)
                  | (white << 1) & NOT_RIGHTMOST & (hp1 << 1 | hp2 << 1 | hp3 << 1 | hp4 << 1)
                  | (white >> 1) & NOT_LEFTMOST & (hp1 >> 1 | hp2 >> 1 | hp3 >> 1 | hp4 >> 1))
         & (hp1 | hp2 | hp3 | hp4)) == 0) {
        return 0; // island rule
    }
    return -1;
}

float State::evaluate() const {

    uint16 blackUp = black << 4 & (hp1 << 4 | hp2 << 4 | hp3 << 4 | hp4 << 4);
    uint16 blackDown = black >> 4 & (hp1 >> 4 | hp2 >> 4 | hp3 >> 4 | hp4 >> 4);
    uint16 blackRight = (black << 1) & NOT_RIGHTMOST & (hp1 << 1 | hp2 << 1 | hp3 << 1 | hp4 << 1);
    uint16 blackLeft = (black >> 1) & NOT_LEFTMOST & (hp1 >> 1 | hp2 >> 1 | hp3 >> 1 | hp4 >> 1);

    uint16 whiteUp = white << 4 & (hp1 << 4 | hp2 << 4 | hp3 << 4 | hp4 << 4);
    uint16 whiteDown = white >> 4 & (hp1 >> 4 | hp2 >> 4 | hp3 >> 4 | hp4 >> 4);
    uint16 whiteRight = (white << 1) & NOT_RIGHTMOST & (hp1 << 1 | hp2 << 1 | hp3 << 1 | hp4 << 1);
    uint16 whiteLeft = (white >> 1) & NOT_LEFTMOST & (hp1 >> 1 | hp2 >> 1 | hp3 >> 1 | hp4 >> 1);

    uint16 superactive = ((black & ((blackUp & blackDown)
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
                                      | (whiteLeft & whiteUp))))
                         & (hp1 | hp2 | hp3 | hp4);

    uint16 hyperactive = ((black & ((blackUp & blackRight & blackLeft)
                                    | (blackDown & blackRight & blackLeft)
                                    | (blackRight & blackUp & blackDown)
                                    | (blackLeft & blackUp & blackDown)))
                          | (white & ((whiteUp & whiteRight & whiteLeft)
                                      | (whiteDown & whiteRight & whiteLeft)
                                      | (whiteRight & whiteUp & whiteDown)
                                      | (whiteLeft & whiteUp & whiteDown))))
                         & (hp1 | hp2 | hp3 | hp4);

    uint16 blackActive = black & active;
    uint16 whiteActive = white & active;

    uint16 faraway = ((black & ~((blackActive << 4)
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
                                   | ((whiteActive >> 2) & NOT_LEFT_HALF))))
                     & (hp1 | hp2 | hp3 | hp4);

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

// #define HISTORY

float State::abBlackSwap(float alpha, float beta, uint8 depth) {
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return (float) endGameScore;
    if (depth == 0) return evaluate();
    for (uint8 move: getBlackSwapMoves()) {
        makeSwap(move);
#ifdef HISTORY
        StateRecord record = StateRecord(black, white, hp1, hp2, hp3, hp4, kings, knights, archers, medics, wizards, shields, stateInfo);
        if (history.find(record) != history.end()) {
            makeSwap(move);
            return 0;
        }
        history.insert(record);
#endif
        float score = abBlackAct(alpha, beta, depth - 1);
#ifdef HISTORY
        history.erase(record);
#endif
        makeSwap(move);
        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}

float State::abBlackSwap(uint8 depth) { return abBlackSwap((float) INT_MIN, (float) INT_MAX, depth); }

pair<float, string> State::abBlackSwapBest(uint8 depth) {
    float alpha = (float) INT_MIN;
    float beta = (float) INT_MAX;
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return pair((float) endGameScore, "game over");
    if (depth == 0) return pair(evaluate(), "zero depth");
    uint8 bestMove = 0;
    for (uint8 move: getBlackSwapMoves()) {
        makeSwap(move);
        float score = abBlackAct(alpha, beta, depth - 1);
        makeSwap(move);
        if (score > alpha) {
            alpha = score;
            bestMove = move;
        }
    }
    return pair(alpha, displaySwap(bestMove));
}

float State::abBlackAct(float alpha, float beta, uint8 depth) {
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return (float) endGameScore;
    if (depth == 0) return evaluate();
    for (int move: getBlackActMoves()) {
        makeBlackAct(move);
#ifdef HISTORY
        StateRecord record = StateRecord(black, white, hp1, hp2, hp3, hp4, kings, knights, archers, medics, wizards, shields, stateInfo);
        if (history.find(record) != history.end()) {
            unmakeBlackAct(move);
            return 0;
        }
        history.insert(record);
#endif
        float score = abWhiteSwap(alpha, beta, depth - 1);
#ifdef HISTORY
        history.erase(record);
#endif
        unmakeBlackAct(move);
        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}

float State::abBlackAct(uint8 depth) { return abBlackAct((float) INT_MIN, (float) INT_MAX, depth); }

pair<float, string> State::abBlackActBest(uint8 depth) {
    float alpha = (float) INT_MIN;
    float beta = (float) INT_MAX;
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return pair((float) endGameScore, "game over");
    if (depth == 0) return pair(evaluate(), "zero depth");
    int bestMove = 0;
    for (int move: getBlackActMoves()) {
        makeBlackAct(move);
        float score = abWhiteSwap(alpha, beta, depth - 1);
        unmakeBlackAct(move);
        if (score > alpha) {
            alpha = score;
            bestMove = move;
        }
    }
    return pair(alpha, displayAct(bestMove));
}

float State::abWhiteSwap(float alpha, float beta, uint8 depth) {
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return (float) endGameScore;
    if (depth == 0) return evaluate();
    for (uint8 move: getWhiteSwapMoves()) {
        makeSwap(move);
#ifdef HISTORY
        StateRecord record = StateRecord(black, white, hp1, hp2, hp3, hp4, kings, knights, archers, medics, wizards, shields, stateInfo);
        if (history.find(record) != history.end()) {
            makeSwap(move);
            return 0;
        }
        history.insert(record);
#endif
        float score = abWhiteAct(alpha, beta, depth - 1);
#ifdef HISTORY
        history.erase(record);
#endif
        makeSwap(move);
        if (score <= alpha) return alpha;
        if (score < beta) beta = score;
    }
    return beta;
}

float State::abWhiteSwap(uint8 depth) { return abWhiteSwap((float) INT_MIN, (float) INT_MAX, depth); }

pair<float, string> State::abWhiteSwapBest(uint8 depth) {
    float alpha = (float) INT_MIN;
    float beta = (float) INT_MAX;
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return pair((float) endGameScore, "game over");
    if (depth == 0) return pair(evaluate(), "zero depth");
    uint8 bestMove = 0;
    for (uint8 move: getWhiteSwapMoves()) {
        makeSwap(move);
        float score = abWhiteAct(alpha, beta, depth - 1);
        makeSwap(move);
        if (score < beta) {
            beta = score;
            bestMove = move;
        }
    }
    return pair(beta, displaySwap(bestMove));
}

float State::abWhiteAct(float alpha, float beta, uint8 depth) {
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return (float) endGameScore;
    if (depth == 0) return evaluate();
    for (int move: getWhiteActMoves()) {
        makeWhiteAct(move);
#ifdef HISTORY
        StateRecord record = StateRecord(black, white, hp1, hp2, hp3, hp4, kings, knights, archers, medics, wizards, shields, stateInfo);
        if (history.find(record) != history.end()) {
            unmakeWhiteAct(move);
            return 0;
        }
        history.insert(record);
#endif
        float score = abBlackSwap(alpha, beta, depth - 1);
#ifdef HISTORY
        history.erase(record);
#endif
        unmakeWhiteAct(move);
        if (score <= alpha) return alpha;
        if (score < beta) beta = score;
    }
    return beta;
}

float State::abWhiteAct(uint8 depth) { return abWhiteAct((float) INT_MIN, (float) INT_MAX, depth); }

pair<float, string> State::abWhiteActBest(uint8 depth) {
    float alpha = (float) INT_MIN;
    float beta = (float) INT_MAX;
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return pair((float) endGameScore, "game over");
    if (depth == 0) return pair(evaluate(), "zero depth");
    int bestMove = 0;
    for (int move: getBlackActMoves()) {
        makeWhiteAct(move);
        float score = abBlackSwap(alpha, beta, depth - 1);
        unmakeWhiteAct(move);
        if (score < beta) {
            beta = score;
            bestMove = move;
        }
    }
    return pair(beta, displayAct(bestMove));
}

#pragma region MiniMax

float State::mmBlackSwap(uint8 depth) {
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return (float) endGameScore;
    if (depth == 0) return evaluate();
    float max = INT_MIN;
    for (uint8 move: getBlackSwapMoves()) {
        makeSwap(move);
#ifdef HISTORY
        StateRecord record = StateRecord(black, white, hp1, hp2, hp3, hp4, kings, knights, archers, medics, wizards, shields, stateInfo);
        if (history.find(record) != history.end()) {
            // cout << 2;
            makeSwap(move);
            return 0;
        }
        history.insert(record);
#endif
        float score = mmBlackAct(depth - 1);
#ifdef HISTORY
        history.erase(record);
#endif
        makeSwap(move);
        if (score > max) max = score;
    }
    return max;
}

float State::mmBlackAct(uint8 depth) {
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return (float) endGameScore;
    if (depth == 0) return evaluate();
    float max = INT_MIN;
    for (int move: getBlackActMoves()) {
        makeBlackAct(move);
#ifdef HISTORY
        StateRecord record = StateRecord(black, white, hp1, hp2, hp3, hp4, kings, knights, archers, medics, wizards, shields, stateInfo);
        if (history.find(record) != history.end()) {
            // cout << 2;
            unmakeBlackAct(move);
            return 0;
        }
        history.insert(record);
#endif
        float score = mmWhiteSwap(depth - 1);
#ifdef HISTORY
        history.erase(record);
#endif
        unmakeBlackAct(move);
        if (score > max) max = score;
    }
    return max;
}

float State::mmWhiteSwap(uint8 depth) {
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return (float) endGameScore;
    if (depth == 0) return evaluate();
    auto min = (float) INT_MAX;
    for (uint8 move: getWhiteSwapMoves()) {
        makeSwap(move);
#ifdef HISTORY
        StateRecord record = StateRecord(black, white, hp1, hp2, hp3, hp4, kings, knights, archers, medics, wizards, shields, stateInfo);
        if (history.find(record) != history.end()) {
            makeSwap(move);
            return 0;
        }
        history.insert(record);
#endif
        float score = mmWhiteAct(depth - 1);
#ifdef HISTORY
        history.erase(record);
#endif
        makeSwap(move);
        if (score < min) min = score;
    }
    return min;
}

float State::mmWhiteAct(uint8 depth) {
    stateCount++;
    int endGameScore = this->endGameScore();
    if (endGameScore != -1) return (float) endGameScore;
    if (depth == 0) return evaluate();
    auto min = (float) INT_MAX;
    for (int move: getWhiteActMoves()) {
        makeWhiteAct(move);
#ifdef HISTORY
        StateRecord record = StateRecord(black, white, hp1, hp2, hp3, hp4, kings, knights, archers, medics, wizards, shields, stateInfo);
        if (history.find(record) != history.end()) {
            // cout << 2;
            unmakeWhiteAct(move);
            return 0;
        }
        history.insert(record);
#endif
        float score = mmBlackSwap(depth - 1);
#ifdef HISTORY
        history.erase(record);
#endif
        unmakeWhiteAct(move);
        if (score < min) min = score;
    }
    return min;
}

#pragma endregion

void State::swap(uint8 piece1, uint8 piece2) {
    black = (((((black >> piece1) & 1) ^ ((black >> piece2) & 1)) << piece1) |
             ((((black >> piece1) & 1) ^ ((black >> piece2) & 1)) << piece2)) ^ black;
    white = (((((white >> piece1) & 1) ^ ((white >> piece2) & 1)) << piece1) |
             ((((white >> piece1) & 1) ^ ((white >> piece2) & 1)) << piece2)) ^ white;
    hp1 = (((((hp1 >> piece1) & 1) ^ ((hp1 >> piece2) & 1)) << piece1) |
           ((((hp1 >> piece1) & 1) ^ ((hp1 >> piece2) & 1)) << piece2)) ^ hp1;
    hp2 = (((((hp2 >> piece1) & 1) ^ ((hp2 >> piece2) & 1)) << piece1) |
           ((((hp2 >> piece1) & 1) ^ ((hp2 >> piece2) & 1)) << piece2)) ^ hp2;
    hp3 = (((((hp3 >> piece1) & 1) ^ ((hp3 >> piece2) & 1)) << piece1) |
           ((((hp3 >> piece1) & 1) ^ ((hp3 >> piece2) & 1)) << piece2)) ^ hp3;
    hp4 = (((((hp4 >> piece1) & 1) ^ ((hp4 >> piece2) & 1)) << piece1) |
           ((((hp4 >> piece1) & 1) ^ ((hp4 >> piece2) & 1)) << piece2)) ^ hp4;
    kings = (((((kings >> piece1) & 1) ^ ((kings >> piece2) & 1)) << piece1) |
             ((((kings >> piece1) & 1) ^ ((kings >> piece2) & 1)) << piece2)) ^ kings;
    knights = (((((knights >> piece1) & 1) ^ ((knights >> piece2) & 1)) << piece1) |
               ((((knights >> piece1) & 1) ^ ((knights >> piece2) & 1)) << piece2)) ^ knights;
    archers = (((((archers >> piece1) & 1) ^ ((archers >> piece2) & 1)) << piece1) |
               ((((archers >> piece1) & 1) ^ ((archers >> piece2) & 1)) << piece2)) ^ archers;
    medics = (((((medics >> piece1) & 1) ^ ((medics >> piece2) & 1)) << piece1) |
              ((((medics >> piece1) & 1) ^ ((medics >> piece2) & 1)) << piece2)) ^ medics;
    wizards = (((((wizards >> piece1) & 1) ^ ((wizards >> piece2) & 1)) << piece1) |
               ((((wizards >> piece1) & 1) ^ ((wizards >> piece2) & 1)) << piece2)) ^ wizards;
    shields = (((((shields >> piece1) & 1) ^ ((shields >> piece2) & 1)) << piece1) |
               ((((shields >> piece1) & 1) ^ ((shields >> piece2) & 1)) << piece2)) ^ shields;
}

void State::updateExtraBitboards() {
    active = ((black & (black << 4 & (hp1 << 4 | hp2 << 4 | hp3 << 4 | hp4 << 4)
                        | black >> 4 & (hp1 >> 4 | hp2 >> 4 | hp3 >> 4 | hp4 >> 4)
                        | (black << 1) & NOT_RIGHTMOST & (hp1 << 1 | hp2 << 1 | hp3 << 1 | hp4 << 1)
                        | (black >> 1) & NOT_LEFTMOST & (hp1 >> 1 | hp2 >> 1 | hp3 >> 1 | hp4 >> 1)))
              | (white & (white << 4 & (hp1 << 4 | hp2 << 4 | hp3 << 4 | hp4 << 4)
                          | white >> 4 & (hp1 >> 4 | hp2 >> 4 | hp3 >> 4 | hp4 >> 4)
                          | (white << 1) & NOT_RIGHTMOST & (hp1 << 1 | hp2 << 1 | hp3 << 1 | hp4 << 1)
                          | (white >> 1) & NOT_LEFTMOST & (hp1 >> 1 | hp2 >> 1 | hp3 >> 1 | hp4 >> 1))))
             & (hp1 | hp2 | hp3 | hp4);
}

void State::makeSwap(uint8 swapMask) {
    swap(swapMask >> 4, swapMask & 0b1111);
    stateInfo = stateInfo ^ 0b10;
    updateExtraBitboards();
}

void State::makeBlackAct(int actMask) {
    switch (actMask & 0b11) {
        case 0:
            stateInfo = (((stateInfo >> 2) + 1) << 2) | (stateInfo & 0b11 ^ 0b11);
            break;
        case 1:
            swap(actMask >> 8, actMask >> 4 & 0b1111);
            stateInfo = stateInfo & 0b11110011 ^ 0b11;
            break;
        case 2:
            hp1 = hp1 & ~(actMask >> 4) | hp2 & actMask >> 4;
            hp2 = hp2 & ~(actMask >> 4) | hp3 & actMask >> 4;
            hp3 = hp3 & ~(actMask >> 4) | hp4 & actMask >> 4;
            hp4 = hp4 & ~(actMask >> 4);
            stateInfo = stateInfo & 0b11110011 ^ 0b11;
            break;
        case 3:
            hp4 = hp4 | (hp3 & actMask >> 4);
            hp3 = hp3 & ~(actMask >> 4) | hp2 & actMask >> 4;
            hp2 = hp2 & ~(actMask >> 4) | hp1 & actMask >> 4;
            hp1 = hp1 & ~(actMask >> 4);
            stateInfo = stateInfo & 0b11110011 ^ 0b11;
            break;
    }
    updateExtraBitboards();
}

void State::unmakeBlackAct(int actMask) {
    switch (actMask & 0b11) {
        case 0:
            stateInfo = (((stateInfo >> 2) - 1) << 2) | (stateInfo & 0b11 ^ 0b11);
            break;
        case 1:
            swap(actMask >> 8, actMask >> 4 & 0b1111);
            stateInfo = ((stateInfo & 0b11110011) + (actMask & 0b1100)) ^ 0b11;
            break;
        case 2:
            hp4 = hp4 | (hp3 & actMask >> 4);
            hp3 = hp3 & ~(actMask >> 4) | hp2 & actMask >> 4;
            hp2 = hp2 & ~(actMask >> 4) | hp1 & actMask >> 4;
            hp1 = hp1 & ~(actMask >> 4) | ~(hp1 | hp2 | hp3 | hp4) & actMask >> 4;
            stateInfo = ((stateInfo & 0b11110011) + (actMask & 0b1100)) ^ 0b11;
            break;
        case 3:
            hp1 = hp1 | hp2 & actMask >> 4;
            hp2 = hp2 & ~(actMask >> 4) | hp3 & actMask >> 4;
            hp3 = hp3 & ~(actMask >> 4) | hp4 & actMask >> 4;
            hp4 = hp4 & ~(actMask >> 4);
            stateInfo = ((stateInfo & 0b11110011) + (actMask & 0b1100)) ^ 0b11;
            break;
    }
    updateExtraBitboards();
}

void State::makeWhiteAct(int actMask) {
    switch (actMask & 0b11) {
        case 0:
            stateInfo = (((stateInfo >> 4) + 1) << 4) | (stateInfo & 0b1111 ^ 0b11);
            break;
        case 1:
            swap(actMask >> 8, actMask >> 4 & 0b1111);
            stateInfo = stateInfo & 0b11001111 ^ 0b11;
             break;
        case 2:
            hp1 = hp1 & ~(actMask >> 4) | hp2 & actMask >> 4;
            hp2 = hp2 & ~(actMask >> 4) | hp3 & actMask >> 4;
            hp3 = hp3 & ~(actMask >> 4) | hp4 & actMask >> 4;
            hp4 = hp4 & ~(actMask >> 4);
            stateInfo = stateInfo & 0b11001111 ^ 0b11;
            break;
        case 3:
            hp4 = hp4 | (hp3 & actMask >> 4);
            hp3 = hp3 & ~(actMask >> 4) | hp2 & actMask >> 4;
            hp2 = hp2 & ~(actMask >> 4) | hp1 & actMask >> 4;
            hp1 = hp1 & ~(actMask >> 4);
            stateInfo = stateInfo & 0b11001111 ^ 0b11;
            break;
    }
    updateExtraBitboards();
}

void State::unmakeWhiteAct(int actMask) {
    switch (actMask & 0b11) {
        case 0:
            stateInfo = (((stateInfo >> 4) - 1) << 4) | (stateInfo & 0b1111 ^ 0b11);
            break;
        case 1:
            swap(actMask >> 8, actMask >> 4 & 0b1111);
            stateInfo = ((stateInfo & 0b11001111) + ((actMask & 0b1100) << 2)) ^ 0b11;
            break;
        case 2:
            hp4 = hp4 | (hp3 & actMask >> 4);
            hp3 = hp3 & ~(actMask >> 4) | hp2 & actMask >> 4;
            hp2 = hp2 & ~(actMask >> 4) | hp1 & actMask >> 4;
            hp1 = hp1 & ~(actMask >> 4) | ~(hp1 | hp2 | hp3 | hp4) & actMask >> 4;
            stateInfo = ((stateInfo & 0b11001111) + ((actMask & 0b1100) << 2)) ^ 0b11;
            break;
        case 3:
            hp1 = hp1 | hp2 & actMask >> 4;
            hp2 = hp2 & ~(actMask >> 4) | hp3 & actMask >> 4;
            hp3 = hp3 & ~(actMask >> 4) | hp4 & actMask >> 4;
            hp4 = hp4 & ~(actMask >> 4);
            stateInfo = ((stateInfo & 0b11001111) + ((actMask & 0b1100) << 2)) ^ 0b11;
            break;
    }
    updateExtraBitboards();
}

vector<pair<State, string>> State::getOffsprings() const {
    vector<pair<State, string>> offsprings = vector<pair<State, string>>();
    vector<uint8> swapMoves = stateInfo & 1 ? getWhiteSwapMoves() : getBlackSwapMoves();
    vector<int> actMoves = stateInfo & 1 ? getWhiteActMoves() : getBlackActMoves();
    switch ((stateInfo >> 1) & 0b1) {
        case SWAP:
            for (uint8 move: swapMoves) {
                State newState = *this;
                newState.makeSwap(move);
                offsprings.emplace_back(newState, displaySwap(move));
            }
            break;
        case ACT:
            for (int move: actMoves) {
                State newState = *this;
                if (stateInfo & 1) newState.makeWhiteAct(move);
                else newState.makeBlackAct(move);
                offsprings.emplace_back(newState, displayAct(move));
            }
            break;
    }
    return offsprings;
}

vector<uint8> State::getBlackSwapMoves() const {
    vector<uint8> moves;
    uint16 swapTargets = (black | white) & ~(white & shields) & (hp1 | hp2 | hp3 | hp4);
    for (auto &pair: swapPairs[black & active]) {
        if ((1 << pair[1] & swapTargets) != 0) {
            moves.push_back((pair[0] << 4) + pair[1]);
        }
    }
    return moves;
}

vector<uint8> State::getWhiteSwapMoves() const {
    vector<uint8> moves;
    uint16 swapTargets = (black | white) & ~(black & shields) & (hp1 | hp2 | hp3 | hp4);
    for (auto &pair: swapPairs[white & active]) {
        if ((1 << pair[1] & swapTargets) != 0) {
            moves.push_back((pair[0] << 4) + pair[1]);
        }
    }
    return moves;
}

vector<int> State::getBlackActMoves() const {
    vector<int> moves;
    uint8 skips = (stateInfo >> 2 & 0b11) << 2;
    uint8 medicIndex;
    uint16 wizardSwap;

    // Double attack
    if (active & black & knights) {
        vector<array<uint8, 2>> targets = knightTargetPairs[
                compressedKnightTargets[
                        meleeTargets[knights & active & black] & white & (hp1 | hp2 | hp3 | hp4)
                ][
                        compressedKnights[active & black & knights]
                ]
        ][compressedKnights[active & black & knights]];
        for (array<uint8, 2> &target: targets) {
            moves.push_back((((1 << target[0]) + (1 << target[1])) << 4) + 0b0010 + skips);
        }
    }

    // Heal
    uint16 medic = active & black & medics;
    if (medic) {
        medicIndex = idx[medic];
        uint16 healTargets = black & (~hp3 | kings | shields) & (hp1 | hp2 | hp3);
        uint16 medicUp = (medic << 4) & healTargets;
        uint16 medicDown = (medic >> 4) & healTargets;
        uint16 medicLeft = medicIndex % 4 != 0 ? (medic >> 1) & healTargets : 0;
        uint16 medicRight = medicIndex % 4 != 3 ? (medic << 1) & healTargets : 0;
        if (medicUp && medicDown && medicLeft && medicRight) {
            moves.push_back(((medicUp | medicDown | medicLeft | medicRight) << 4) + 0b0011 + skips);
            // goto medicEnd;
        }
        if (medicUp && medicDown && medicLeft) {
            moves.push_back(((medicUp | medicDown | medicLeft) << 4) + 0b0011 + skips);
            // goto medicEnd;
        }
        if (medicUp && medicDown && medicRight) {
            moves.push_back(((medicUp | medicDown | medicRight) << 4) + 0b0011 + skips);
            // goto medicEnd;
        }
        if (medicUp && medicLeft && medicRight) {
            moves.push_back(((medicUp | medicLeft | medicRight) << 4) + 0b0011 + skips);
            // goto medicEnd;
        }
        if (medicDown && medicLeft && medicRight) {
            moves.push_back(((medicDown | medicLeft | medicRight) << 4) + 0b0011 + skips);
            // goto medicEnd;
        }
        if (medicUp && medicDown) {
            moves.push_back(((medicUp | medicDown) << 4) + 0b0011 + skips);
            // goto medicEnd;
        }
        if (medicUp && medicLeft) {
            moves.push_back(((medicUp | medicLeft) << 4) + 0b0011 + skips);
            // goto medicEnd;
        }
        if (medicUp && medicRight) {
            moves.push_back(((medicUp | medicRight) << 4) + 0b0011 + skips);
            // goto medicEnd;
        }
        if (medicDown && medicLeft) {
            moves.push_back(((medicDown | medicLeft) << 4) + 0b0011 + skips);
            // goto medicEnd;
        }
        if (medicDown && medicRight) {
            moves.push_back(((medicDown | medicRight) << 4) + 0b0011 + skips);
            // goto medicEnd;
        }
        if (medicLeft && medicRight) {
            moves.push_back(((medicLeft | medicRight) << 4) + 0b0011 + skips);
            // goto medicEnd;
        }
        if (medicUp) {
            moves.push_back((medicUp << 4) + 0b0011 + skips);
            // goto medicEnd;
        }
        if (medicDown) {
            moves.push_back((medicDown << 4) + 0b0011 + skips);
            // goto medicEnd;
        }
        if (medicLeft) {
            moves.push_back((medicLeft << 4) + 0b0011 + skips);
            // goto medicEnd;
        }
        if (medicRight) {
            moves.push_back((medicRight << 4) + 0b0011 + skips);
            // goto medicEnd;
        }
    }
    // medicEnd:

    // Single attack
    uint16 underAttack = (meleeTargets[(kings | knights | archers) & active & black] |
                          archerTargets[archers & active & black][idx[shields & white & (hp1 | hp2 | hp3 | hp4)]])
                         & white & (hp1 | hp2 | hp3 | hp4);
    for (uint8 i = 0; underAttack != 0; underAttack >>= 1) {
        if (underAttack & 1) {
            moves.push_back((1 << i << 4) + 0b0010 + skips);
        }
        i++;
    }

    // Teleport
    if (active & black & wizards
            && (black | white) & (hp1 | hp2 | hp3 | hp4)) {
        wizardSwap = black & ~wizards & (hp1 | hp2 | hp3 | hp4);
        for (uint8 i = 0; wizardSwap != 0; wizardSwap >>= 1) {
            if (wizardSwap & 1) {
                moves.push_back((idx[black & wizards] << 8) + (i << 4) + 0b0001 + skips);
            }
            i++;
        }
    }

    // Skip
    moves.push_back(0);

    return moves;
}

vector<int> State::getWhiteActMoves() const {
    vector<int> moves;
    uint8 skips = (stateInfo >> 4 & 0b11) << 2;
    uint8 medicIndex;
    uint16 wizardSwap;

    // Double attack
    if (active & white & knights) {
        vector<array<uint8, 2>> targets = knightTargetPairs[
                compressedKnightTargets[
                        meleeTargets[knights & active & white] & black & (hp1 | hp2 | hp3 | hp4)
                ][
                        compressedKnights[active & white & knights]
                ]
        ][compressedKnights[active & white & knights]];
        for (auto &target: targets) {
            moves.push_back((((1 << target[0]) + (1 << target[1])) << 4) + 0b0010 + skips);
        }
    }

    // Heal
    uint16 medic = active & white & medics;
    if (medic) {
        medicIndex = idx[medic];
        uint16 healTargets = white & (~hp3 | kings | shields) & (hp1 | hp2 | hp3);
        uint16 medicUp = (medic << 4) & healTargets;
        uint16 medicDown = (medic >> 4) & healTargets;
        uint16 medicLeft = medicIndex % 4 != 0 ? (medic >> 1) & healTargets : 0;
        uint16 medicRight = medicIndex % 4 != 3 ? (medic << 1) & healTargets : 0;
        if (medicUp && medicDown && medicLeft && medicRight) {
            moves.push_back(((medicUp | medicDown | medicLeft | medicRight) << 4) + 0b0011 + skips);
        }
        if (medicUp && medicDown && medicLeft) {
            moves.push_back(((medicUp | medicDown | medicLeft) << 4) + 0b0011 + skips);
        }
        if (medicUp && medicDown && medicRight) {
            moves.push_back(((medicUp | medicDown | medicRight) << 4) + 0b0011 + skips);
        }
        if (medicUp && medicLeft && medicRight) {
            moves.push_back(((medicUp | medicLeft | medicRight) << 4) + 0b0011 + skips);
        }
        if (medicDown && medicLeft && medicRight) {
            moves.push_back(((medicDown | medicLeft | medicRight) << 4) + 0b0011 + skips);
        }
        if (medicUp && medicDown) {
            moves.push_back(((medicUp | medicDown) << 4) + 0b0011 + skips);
        }
        if (medicUp && medicLeft) {
            moves.push_back(((medicUp | medicLeft) << 4) + 0b0011 + skips);
        }
        if (medicUp && medicRight) {
            moves.push_back(((medicUp | medicRight) << 4) + 0b0011 + skips);
        }
        if (medicDown && medicLeft) {
            moves.push_back(((medicDown | medicLeft) << 4) + 0b0011 + skips);
        }
        if (medicDown && medicRight) {
            moves.push_back(((medicDown | medicRight) << 4) + 0b0011 + skips);
        }
        if (medicLeft && medicRight) {
            moves.push_back(((medicLeft | medicRight) << 4) + 0b0011 + skips);
        }
        if (medicUp) {
            moves.push_back((medicUp << 4) + 0b0011 + skips);
        }
        if (medicDown) {
            moves.push_back((medicDown << 4) + 0b0011 + skips);
        }
        if (medicLeft) {
            moves.push_back((medicLeft << 4) + 0b0011 + skips);
        }
        if (medicRight) {
            moves.push_back((medicRight << 4) + 0b0011 + skips);
        }
    }

    // Single attack
    uint16 underAttack = (meleeTargets[(kings | knights | archers) & active & white] |
                          archerTargets[archers & active & white][idx[shields & black & (hp1 | hp2 | hp3 | hp4)]])
                         & black & (hp1 | hp2 | hp3 | hp4);
    for (uint8 i = 0; underAttack != 0; underAttack >>= 1) {
        if (underAttack & 1) {
            moves.push_back((1 << i << 4) + 0b0010 + skips);
        }
        i++;
    }

    // Teleport
    if (active & white & wizards) {
        wizardSwap = white & ~wizards & (hp1 | hp2 | hp3 | hp4);
        for (uint8 i = 0; wizardSwap != 0; wizardSwap >>= 1) {
            if (wizardSwap & 1) {
                moves.push_back((idx[white & wizards] << 8) + (i << 4) + 0b0001 + skips);
            }
            i++;
        }
    }

    // Skip
    moves.push_back(0);

    return moves;
}

string State::display() const {
    string display;
    char hp;
    char type;
    for (int i = 12; i >= 0; i++) {
        if (hp1 & 1 << i) {
            hp = '1';
        } else if (hp2 & 1 << i) {
            hp = '2';
        } else if (hp3 & 1 << i) {
            hp = '3';
        } else if (hp4 & 1 << i) {
            hp = '4';
        } else {
            display += "__";
            if (i % 4 == 3) {
                display += '\n';
                i -= 8;
            } else {
                display += ' ';
            }
            continue;
        }
        if (kings & 1 << i) {
            type = 'k';
        } else if (knights & 1 << i) {
            type = 'n';
        } else if (archers & 1 << i) {
            type = 'a';
        } else if (medics & 1 << i) {
            type = 'm';
        } else if (wizards & 1 << i) {
            type = 'w';
        } else if (shields & 1 << i) {
            type = 's';
        }
        if (black & 1 << i) {
            type -= 32;
        }
        display += type;
        display += hp;
        if (i % 4 == 3) {
            display += '\n';
            i -= 8;
        } else {
            display += ' ';
        }
    }
    display += stateInfo & 0b1 ? "white " : "black ";
    display += stateInfo & 0b10 ? "act " : "swap ";
    display += std::to_string((int) ((stateInfo & 0b1100) >> 2));
    display += ' ';
    display += std::to_string((int) ((stateInfo & 0b110000) >> 4));
    display += '\n';
    return display;
}

void State::resetGlobals() {
    stateCount = 0;
    history.clear();
}
