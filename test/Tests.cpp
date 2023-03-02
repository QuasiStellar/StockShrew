#define CATCH_CONFIG_MAIN

#include <algorithm>

#include "../src/State.h"

#include "catch.hpp"

#include "Abbreviations.h"
#include "TestUtils.h"

using std::cout, std::endl, std::array, std::vector, std::bitset, std::find_if;

TEST_CASE("Minimax depth 1", "[SearchTests]") {
    setUp();
    Piece *board[4][4] = {
            {a3 m3 a3 k4},
            {w3 n3 n3 s4},
            {S3 N3 N3 W3},
            {K4 A3 M3 A3}
    };
    auto state = State(board, Side::Black, Phase::Swap, 0, 0);
    state.search(1, Strategy::MiniMax);
    CHECK(State::stateCount == 14);
    State::resetGlobals();
}

TEST_CASE("Minimax depth 2", "[SearchTests]") {
    setUp();
    Piece *board[4][4] = {
            {a3 m3 a3 k4},
            {w3 n3 n3 s4},
            {S3 N3 N3 W3},
            {K4 A3 M3 A3}
    };
    auto state = State(board, Side::Black, Phase::Swap, 0, 0);
    state.search(2, Strategy::MiniMax);
    CHECK(State::stateCount == 175);
    State::resetGlobals();
}

TEST_CASE("Minimax depth 3", "[SearchTests]") {
    setUp();
    Piece *board[4][4] = {
            {a3 m3 a3 k4},
            {w3 n3 n3 s4},
            {S3 N3 N3 W3},
            {K4 A3 M3 A3}
    };
    auto state = State(board, Side::Black, Phase::Swap, 0, 0);
    state.search(3, Strategy::MiniMax);
    CHECK(State::stateCount == 2062);
    State::resetGlobals();
}

TEST_CASE("Minimax depth 4", "[SearchTests]") {
    setUp();
    Piece *board[4][4] = {
            {a3 m3 a3 k4},
            {w3 n3 n3 s4},
            {S3 N3 N3 W3},
            {K4 A3 M3 A3}
    };
    auto state = State(board, Side::Black, Phase::Swap, 0, 0);
    state.search(4, Strategy::MiniMax);
    CHECK(State::stateCount == 25689);
    State::resetGlobals();
}

TEST_CASE("Minimax depth 5", "[SearchTests]") {
    setUp();
    Piece *board[4][4] = {
            {a3 m3 a3 k4},
            {w3 n3 n3 s4},
            {S3 N3 N3 W3},
            {K4 A3 M3 A3}
    };
    auto state = State(board, Side::Black, Phase::Swap, 0, 0);
    state.search(5, Strategy::MiniMax);
    CHECK(State::stateCount == 304020);
    State::resetGlobals();
}

TEST_CASE("Minimax depth 6", "[SearchTests]") {
    setUp();
    Piece *board[4][4] = {
            {a3 m3 a3 k4},
            {w3 n3 n3 s4},
            {S3 N3 N3 W3},
            {K4 A3 M3 A3}
    };
    auto state = State(board, Side::Black, Phase::Swap, 0, 0);
    state.search(6, Strategy::MiniMax);
    CHECK(State::stateCount == 3951293);
    State::resetGlobals();
}

TEST_CASE("Island rule test", "[SearchTests]") {
    setUp();
    Piece *board[4][4] = {
            {k1 a1 __ __},
            {__ W1 __ __},
            {S3 N3 N3 __},
            {K4 A3 M3 A3}
    };
    auto state = State(board, Side::White, Phase::Act, 0, 0);
    auto result = state.search(6, Strategy::MiniMax);
    CHECK(result == 0);
    State::resetGlobals();
}

TEST_CASE("Swap test", "[OffspringTests]") {
    setUp();
    Piece *board[4][4] = {
            {w3 __ M3 s1},
            {m3 n2 __ A3},
            {a3 S2 __ k4},
            {K4 N3 a3 W3}
    };
    auto state = State(board, Side::White, Phase::Swap, 0, 0);
    vector<uint8> moves = state.getWhiteSwapMoves();
    vector<State> offsprings = vector<State>();
    for (auto move : moves) {
        State newState = state;
        newState.makeSwap(move);
        CHECK(newState.hash != state.hash);
        cout << newState.display() << endl;
        offsprings.push_back(newState);
    }

    CHECK((int) offsprings.size() == 4);

    Piece *board1[4][4] = {
            {m3 __ M3 s1},
            {w3 n2 __ A3},
            {a3 S2 __ k4},
            {K4 N3 a3 W3}
    };
    CHECK(contains(offsprings, State(board1, Side::White, Phase::Act, 0, 0)));

    Piece *board2[4][4] = {
            {w3 __ M3 s1},
            {n2 m3 __ A3},
            {a3 S2 __ k4},
            {K4 N3 a3 W3}
    };
    CHECK(contains(offsprings, State(board2, Side::White, Phase::Act, 0, 0)));

    Piece *board3[4][4] = {
            {w3 __ M3 s1},
            {a3 n2 __ A3},
            {m3 S2 __ k4},
            {K4 N3 a3 W3}
    };
    CHECK(contains(offsprings, State(board3, Side::White, Phase::Act, 0, 0)));

    Piece *board4[4][4] = {
            {w3 __ M3 s1},
            {m3 n2 __ A3},
            {K4 S2 __ k4},
            {a3 N3 a3 W3}
    };
    CHECK(contains(offsprings, State(board4, Side::White, Phase::Act, 0, 0)));

    for (int i = 0; i < 4; i++) {
        offsprings[i].makeSwap(moves[i]);
        CHECK(offsprings[i].display() == state.display());
        CHECK(offsprings[i].hash == state.hash);
    }
}

TEST_CASE("Swap test 2", "[OffspringTests]") {
    setUp();
    Piece *board[4][4] = {
            {a3 m3 a3 k4},
            {w3 n3 n3 s4},
            {S3 N3 N3 W3},
            {K4 A3 M3 A3}
    };
    auto state = State(board, Side::Black, Phase::Swap, 0, 0);
    vector<uint8> moves = state.getBlackSwapMoves();
    vector<State> offsprings = vector<State>();
    for (auto move : moves) {
        State newState = state;
        newState.makeSwap(move);
        CHECK(newState.hash != state.hash);
        cout << newState.display() << endl;
        offsprings.push_back(newState);
    }
    CHECK((int) offsprings.size() == 13);
}

TEST_CASE("Knight test", "[OffspringTests]") {
    setUp();
    Piece *board[4][4] = {
            {__ __ __ __},
            {__ n3 A3 __},
            {__ k2 N1 w1},
            {K4 N3 a1 s2}
    };
    auto state = State(board, Side::Black, Phase::Act, 0, 0);
    vector<int> moves = state.getBlackActMoves();
    vector<State> offsprings = vector<State>();
    for (auto move : moves) {
        State newState = state;
        newState.makeBlackAct(move);
        CHECK(newState.hash != state.hash);
        cout << newState.display() << endl;
        offsprings.push_back(newState);
    }
    CHECK((int) offsprings.size() == 8);

    Piece *board1[4][4] = {
            {__ __ __ __},
            {__ n3 A3 __},
            {__ k1 N1 w1},
            {K4 N3 __ s2}
    };
    CHECK(contains(offsprings, State(board1, Side::White, Phase::Swap, 0, 0)));

    Piece *board2[4][4] = {
            {__ __ __ __},
            {__ n3 A3 __},
            {__ k2 N1 __},
            {K4 N3 __ s2}
    };
    CHECK(contains(offsprings, State(board2, Side::White, Phase::Swap, 0, 0)));

    Piece *board3[4][4] = {
            {__ __ __ __},
            {__ n3 A3 __},
            {__ k1 N1 __},
            {K4 N3 a1 s2}
    };
    CHECK(contains(offsprings, State(board3, Side::White, Phase::Swap, 0, 0)));

    Piece *board4[4][4] = {
            {__ __ __ __},
            {__ n3 A3 __},
            {__ k1 N1 w1},
            {K4 N3 a1 s2}
    };
    CHECK(contains(offsprings, State(board4, Side::White, Phase::Swap, 0, 0)));

    Piece *board5[4][4] = {
            {__ __ __ __},
            {__ n3 A3 __},
            {__ k2 N1 w1},
            {K4 N3 __ s2}
    };
    CHECK(contains(offsprings, State(board5, Side::White, Phase::Swap, 0, 0)));

    Piece *board6[4][4] = {
            {__ __ __ __},
            {__ n3 A3 __},
            {__ k2 N1 __},
            {K4 N3 a1 s2}
    };
    CHECK(contains(offsprings, State(board6, Side::White, Phase::Swap, 0, 0)));

    Piece *board7[4][4] = {
            {__ __ __ __},
            {__ n2 A3 __},
            {__ k2 N1 w1},
            {K4 N3 a1 s2}
    };
    CHECK(contains(offsprings, State(board7, Side::White, Phase::Swap, 0, 0)));

    Piece *board8[4][4] = {
            {__ __ __ __},
            {__ n3 A3 __},
            {__ k2 N1 w1},
            {K4 N3 a1 s2}
    };
    CHECK(contains(offsprings, State(board8, Side::White, Phase::Swap, 1, 0)));

    for (int i = 0; i < 8; i++) {
        offsprings[i].unmakeBlackAct(moves[i]);
        CHECK(offsprings[i].display() == state.display());
        CHECK(offsprings[i].hash == state.hash);
    }
}

TEST_CASE("Archer test", "[OffspringTests]") {
    setUp();
    Piece *board[4][4] = {
            {n2 k4 __ m3},
            {__ s1 __ __},
            {n2 A3 __ w1},
            {M1 K4 N3 A2}
    };
    auto state = State(board, Side::Black, Phase::Act, 0, 0);
    vector<int> moves = state.getBlackActMoves();
    vector<State> offsprings = vector<State>();
    for (auto move : moves) {
        State newState = state;
        newState.makeBlackAct(move);
        CHECK(newState.hash != state.hash);
        cout << newState.display() << endl;
        offsprings.push_back(newState);
    }
    CHECK((int) offsprings.size() == 5);

    Piece *board1[4][4] = {
            {n2 k4 __ m3},
            {__ s1 __ __},
            {n1 A3 __ w1},
            {M1 K4 N3 A2}
    };
    CHECK(contains(offsprings, State(board1, Side::White, Phase::Swap, 0, 0)));

    Piece *board2[4][4] = {
            {n2 k4 __ m3},
            {__ __ __ __},
            {n2 A3 __ w1},
            {M1 K4 N3 A2}
    };
    CHECK(contains(offsprings, State(board2, Side::White, Phase::Swap, 0, 0)));

    Piece *board3[4][4] = {
            {n2 k4 __ m3},
            {__ s1 __ __},
            {n2 A3 __ __},
            {M1 K4 N3 A2}
    };
    CHECK(contains(offsprings, State(board3, Side::White, Phase::Swap, 0, 0)));

    Piece *board4[4][4] = {
            {n2 k4 __ m2},
            {__ s1 __ __},
            {n2 A3 __ w1},
            {M1 K4 N3 A2}
    };
    CHECK(contains(offsprings, State(board4, Side::White, Phase::Swap, 0, 0)));

    Piece *board5[4][4] = {
            {n2 k4 __ m3},
            {__ s1 __ __},
            {n2 A3 __ w1},
            {M1 K4 N3 A2}
    };
    CHECK(contains(offsprings, State(board5, Side::White, Phase::Swap, 1, 0)));

    for (int i = 0; i < 5; i++) {
        offsprings[i].unmakeBlackAct(moves[i]);
        CHECK(offsprings[i].display() == state.display());
        CHECK(offsprings[i].hash == state.hash);
    }
}

TEST_CASE("Archer test 2", "[OffspringTests]") {
    setUp();
    Piece *board[4][4] = {
            {n2 k4 __ m3},
            {__ a1 __ __},
            {__ A3 s1 w1},
            {M1 K4 __ __}
    };
    auto state = State(board, Side::Black, Phase::Act, 2, 2);
    vector<int> moves = state.getBlackActMoves();
    vector<State> offsprings = vector<State>();
    for (auto move : moves) {
        State newState = state;
        newState.makeBlackAct(move);
        CHECK(newState.hash != state.hash);
        cout << newState.display() << endl;
        offsprings.push_back(newState);
    }
    CHECK((int) offsprings.size() == 4);

    Piece *board1[4][4] = {
            {n2 k4 __ m3},
            {__ __ __ __},
            {__ A3 s1 w1},
            {M1 K4 __ __}
    };
    CHECK(contains(offsprings, State(board1, Side::White, Phase::Swap, 0, 2)));

    Piece *board2[4][4] = {
            {n2 k3 __ m3},
            {__ a1 __ __},
            {__ A3 s1 w1},
            {M1 K4 __ __}
    };
    CHECK(contains(offsprings, State(board2, Side::White, Phase::Swap, 0, 2)));

    Piece *board3[4][4] = {
            {n2 k4 __ m3},
            {__ a1 __ __},
            {__ A3 __ w1},
            {M1 K4 __ __}
    };
    CHECK(contains(offsprings, State(board3, Side::White, Phase::Swap, 0, 2)));

    Piece *board4[4][4] = {
            {n2 k4 __ m3},
            {__ a1 __ __},
            {__ A3 s1 w1},
            {M1 K4 __ __}
    };
    CHECK(contains(offsprings, State(board4, Side::White, Phase::Swap, 3, 2)));

    for (int i = 0; i < 4; i++) {
        offsprings[i].unmakeBlackAct(moves[i]);
        CHECK(offsprings[i].display() == state.display());
        CHECK(offsprings[i].hash == state.hash);
    }
}

TEST_CASE("Medic test", "[OffspringTests]") {
    setUp();
    Piece *board[4][4] = {
            {__ __ __ k3},
            {__ N3 __ w1},
            {A1 M3 N1 __},
            {__ K3 __ __}
    };
    auto state = State(board, Side::Black, Phase::Act, 0, 0);
    vector<int> moves = state.getBlackActMoves();
    vector<State> offsprings = vector<State>();
    for (auto move : moves) {
        State newState = state;
        newState.makeBlackAct(move);
        CHECK(newState.hash != state.hash);
        cout << newState.display() << endl;
        offsprings.push_back(newState);
    }
    CHECK((int) offsprings.size() == 8);

    Piece *board1[4][4] = {
            {__ __ __ k3},
            {__ N3 __ w1},
            {A2 M3 N1 __},
            {__ K3 __ __}
    };
    CHECK(contains(offsprings, State(board1, Side::White, Phase::Swap, 0, 0)));

    Piece *board2[4][4] = {
            {__ __ __ k3},
            {__ N3 __ w1},
            {A1 M3 N1 __},
            {__ K4 __ __}
    };
    CHECK(contains(offsprings, State(board2, Side::White, Phase::Swap, 0, 0)));

    Piece *board3[4][4] = {
            {__ __ __ k3},
            {__ N3 __ w1},
            {A1 M3 N2 __},
            {__ K3 __ __}
    };
    CHECK(contains(offsprings, State(board3, Side::White, Phase::Swap, 0, 0)));

    Piece *board4[4][4] = {
            {__ __ __ k3},
            {__ N3 __ w1},
            {A2 M3 N1 __},
            {__ K4 __ __}
    };
    CHECK(contains(offsprings, State(board4, Side::White, Phase::Swap, 0, 0)));

    Piece *board5[4][4] = {
            {__ __ __ k3},
            {__ N3 __ w1},
            {A2 M3 N2 __},
            {__ K3 __ __}
    };
    CHECK(contains(offsprings, State(board5, Side::White, Phase::Swap, 0, 0)));

    Piece *board6[4][4] = {
            {__ __ __ k3},
            {__ N3 __ w1},
            {A1 M3 N2 __},
            {__ K4 __ __}
    };
    CHECK(contains(offsprings, State(board6, Side::White, Phase::Swap, 0, 0)));

    Piece *board7[4][4] = {
            {__ __ __ k3},
            {__ N3 __ w1},
            {A2 M3 N2 __},
            {__ K4 __ __}
    };
    CHECK(contains(offsprings, State(board7, Side::White, Phase::Swap, 0, 0)));

    Piece *board8[4][4] = {
            {__ __ __ k3},
            {__ N3 __ w1},
            {A1 M3 N1 __},
            {__ K3 __ __}
    };
    CHECK(contains(offsprings, State(board8, Side::White, Phase::Swap, 1, 0)));

    for (int i = 0; i < 8; i++) {
        offsprings[i].unmakeBlackAct(moves[i]);
        CHECK(offsprings[i].display() == state.display());
        CHECK(offsprings[i].hash == state.hash);
    }
}

TEST_CASE("Wizard test", "[OffspringTests]") {
    setUp();
    Piece *board[4][4] = {
            {n2 k4 __ N3},
            {__ s1 __ __},
            {n2 M3 __ w1},
            {W1 K4 N3 S2}
    };
    auto state = State(board, Side::Black, Phase::Act, 0, 0);
    vector<int> moves = state.getBlackActMoves();
    vector<State> offsprings = vector<State>();
    for (auto move : moves) {
        State newState = state;
        newState.makeBlackAct(move);
        CHECK(newState.hash != state.hash);
        cout << newState.display() << endl;
        offsprings.push_back(newState);
    }
    CHECK((int) offsprings.size() == 6);

    Piece *board1[4][4] = {
            {n2 k4 __ N3},
            {__ s1 __ __},
            {n2 M3 __ w1},
            {K4 W1 N3 S2}
    };
    CHECK(contains(offsprings, State(board1, Side::White, Phase::Swap, 0, 0)));

    Piece *board2[4][4] = {
            {n2 k4 __ N3},
            {__ s1 __ __},
            {n2 W1 __ w1},
            {M3 K4 N3 S2}
    };
    CHECK(contains(offsprings, State(board2, Side::White, Phase::Swap, 0, 0)));

    Piece *board3[4][4] = {
            {n2 k4 __ N3},
            {__ s1 __ __},
            {n2 M3 __ w1},
            {N3 K4 W1 S2}
    };
    CHECK(contains(offsprings, State(board3, Side::White, Phase::Swap, 0, 0)));

    Piece *board4[4][4] = {
            {n2 k4 __ N3},
            {__ s1 __ __},
            {n2 M3 __ w1},
            {S2 K4 N3 W1}
    };
    CHECK(contains(offsprings, State(board4, Side::White, Phase::Swap, 0, 0)));

    Piece *board5[4][4] = {
            {n2 k4 __ W1},
            {__ s1 __ __},
            {n2 M3 __ w1},
            {N3 K4 N3 S2}
    };
    CHECK(contains(offsprings, State(board5, Side::White, Phase::Swap, 0, 0)));

    Piece *board6[4][4] = {
            {n2 k4 __ N3},
            {__ s1 __ __},
            {n2 M3 __ w1},
            {W1 K4 N3 S2}
    };
    CHECK(contains(offsprings, State(board6, Side::White, Phase::Swap, 1, 0)));

    for (int i = 0; i < 6; i++) {
        offsprings[i].unmakeBlackAct(moves[i]);
        CHECK(offsprings[i].display() == state.display());
        CHECK(offsprings[i].hash == state.hash);
    }
}

TEST_CASE("White test", "[OffspringTests]") {
    setUp();
    Piece *board[4][4] = {
            {w2 m3 __ N3},
            {k1 a1 S1 A2},
            {n2 M3 __ __},
            {W1 K4 N3 __}
    };
    auto state = State(board, Side::White, Phase::Act, 2, 1);
    vector<int> moves = state.getWhiteActMoves();
    vector<State> offsprings = vector<State>();
    for (auto move: moves) {
        State newState = state;
        newState.makeWhiteAct(move);
        CHECK(newState.hash != state.hash);
        cout << newState.display() << endl;
        offsprings.push_back(newState);
    }
    CHECK((int) offsprings.size() == 13);

    Piece *board1[4][4] = {
            {w2 m3 __ N3},
            {k1 a1 S1 A2},
            {n2 M2 __ __},
            {__ K4 N3 __}
    };
    CHECK(contains(offsprings, State(board1, Side::Black, Phase::Swap, 2, 0)));

    Piece *board2[4][4] = {
            {w3 m3 __ N3},
            {k1 a2 S1 A2},
            {n2 M3 __ __},
            {W1 K4 N3 __}
    };
    CHECK(contains(offsprings, State(board2, Side::Black, Phase::Swap, 2, 0)));

    Piece *board3[4][4] = {
            {w2 m3 __ N3},
            {k1 a2 S1 A2},
            {n2 M3 __ __},
            {W1 K4 N3 __}
    };
    CHECK(contains(offsprings, State(board3, Side::Black, Phase::Swap, 2, 0)));

    Piece *board4[4][4] = {
            {w3 m3 __ N3},
            {k1 a1 S1 A2},
            {n2 M3 __ __},
            {W1 K4 N3 __}
    };
    CHECK(contains(offsprings, State(board4, Side::Black, Phase::Swap, 2, 0)));

    Piece *board5[4][4] = {
            {w2 m3 __ N3},
            {k1 a1 S1 A2},
            {n2 M3 __ __},
            {__ K4 N3 __}
    };
    CHECK(contains(offsprings, State(board5, Side::Black, Phase::Swap, 2, 0)));

    Piece *board6[4][4] = {
            {w2 m3 __ N3},
            {k1 a1 S1 A2},
            {n2 M3 __ __},
            {W1 K3 N3 __}
    };
    CHECK(contains(offsprings, State(board6, Side::Black, Phase::Swap, 2, 0)));

    Piece *board7[4][4] = {
            {w2 m3 __ N3},
            {k1 a1 S1 A2},
            {n2 M2 __ __},
            {W1 K4 N3 __}
    };
    CHECK(contains(offsprings, State(board7, Side::Black, Phase::Swap, 2, 0)));

    Piece *board8[4][4] = {
            {w2 m3 __ N3},
            {k1 a1 __ A2},
            {n2 M3 __ __},
            {W1 K4 N3 __}
    };
    CHECK(contains(offsprings, State(board8, Side::Black, Phase::Swap, 2, 0)));

    Piece *board9[4][4] = {
            {n2 m3 __ N3},
            {k1 a1 S1 A2},
            {w2 M3 __ __},
            {W1 K4 N3 __}
    };
    CHECK(contains(offsprings, State(board9, Side::Black, Phase::Swap, 2, 0)));

    Piece *board10[4][4] = {
            {k1 m3 __ N3},
            {w2 a1 S1 A2},
            {n2 M3 __ __},
            {W1 K4 N3 __}
    };
    CHECK(contains(offsprings, State(board10, Side::Black, Phase::Swap, 2, 0)));

    Piece *board11[4][4] = {
            {a1 m3 __ N3},
            {k1 w2 S1 A2},
            {n2 M3 __ __},
            {W1 K4 N3 __}
    };
    CHECK(contains(offsprings, State(board11, Side::Black, Phase::Swap, 2, 0)));

    Piece *board12[4][4] = {
            {m3 w2 __ N3},
            {k1 a1 S1 A2},
            {n2 M3 __ __},
            {W1 K4 N3 __}
    };
    CHECK(contains(offsprings, State(board12, Side::Black, Phase::Swap, 2, 0)));

    Piece *board13[4][4] = {
            {w2 m3 __ N3},
            {k1 a1 S1 A2},
            {n2 M3 __ __},
            {W1 K4 N3 __}
    };
    CHECK(contains(offsprings, State(board13, Side::Black, Phase::Swap, 2, 2)));

    for (int i = 0; i < 13; i++) {
        offsprings[i].unmakeWhiteAct(moves[i]);
        CHECK(offsprings[i].display() == state.display());
        CHECK(offsprings[i].hash == state.hash);
    }
}

TEST_CASE("White test 2", "[OffspringTests]") {
    setUp();
    Piece *board[4][4] = {
            {k4 a2 N3 n2},
            {s2 N3 K4 __},
            {__ __ __ __},
            {__ __ __ __}
    };
    auto state = State(board, Side::Black, Phase::Act, 0, 0);
    vector<int> moves = state.getBlackActMoves();
    vector<State> offsprings = vector<State>();
    for (auto move: moves) {
        State newState = state;
        newState.makeBlackAct(move);
        CHECK(newState.hash != state.hash);
        cout << newState.display() << endl;
        offsprings.push_back(newState);
    }
    CHECK((int) offsprings.size() == 6);

    Piece *board1[4][4] = {
            {k4 a1 N3 n1},
            {s2 N3 K4 __},
            {__ __ __ __},
            {__ __ __ __}
    };
    CHECK(contains(offsprings, State(board1, Side::White, Phase::Swap, 0, 0)));

    Piece *board2[4][4] = {
            {k4 a1 N3 n2},
            {s1 N3 K4 __},
            {__ __ __ __},
            {__ __ __ __}
    };
    CHECK(contains(offsprings, State(board2, Side::White, Phase::Swap, 0, 0)));

    Piece *board3[4][4] = {
            {k4 a2 N3 n2},
            {s1 N3 K4 __},
            {__ __ __ __},
            {__ __ __ __}
    };
    CHECK(contains(offsprings, State(board3, Side::White, Phase::Swap, 0, 0)));

    Piece *board4[4][4] = {
            {k4 a1 N3 n2},
            {s2 N3 K4 __},
            {__ __ __ __},
            {__ __ __ __}
    };
    CHECK(contains(offsprings, State(board4, Side::White, Phase::Swap, 0, 0)));

    Piece *board5[4][4] = {
            {k4 a2 N3 n1},
            {s2 N3 K4 __},
            {__ __ __ __},
            {__ __ __ __}
    };
    CHECK(contains(offsprings, State(board5, Side::White, Phase::Swap, 0, 0)));

    Piece *board6[4][4] = {
            {k4 a2 N3 n2},
            {s2 N3 K4 __},
            {__ __ __ __},
            {__ __ __ __}
    };
    CHECK(contains(offsprings, State(board6, Side::White, Phase::Swap, 1, 0)));

    for (int i = 0; i < 6; i++) {
        offsprings[i].unmakeBlackAct(moves[i]);
        CHECK(offsprings[i].display() == state.display());
        CHECK(offsprings[i].hash == state.hash);
    }
}
