#define CATCH_CONFIG_MAIN

#include <algorithm>
#include "catch.hpp"
#include "../src/State.h"
#include "../src/Display.h"
#include "Abbreviations.h"

using std::cout, std::endl, std::array, std::vector, std::bitset, std::find_if;

bool contains(const vector<State> &list, const State &subject) {
    auto iter = find_if(list.begin(), list.end(), [&](const State &state) {
        return state.black == subject.black
               && state.white == subject.white
               && state.active == subject.active
               && state.superactive == subject.superactive
               && state.hyperactive == subject.hyperactive
               && state.faraway == subject.faraway
               && state.hp1 == subject.hp1
               && state.hp2 == subject.hp2
               && state.hp3 == subject.hp3
               && state.hp4 == subject.hp4
               && state.kings == subject.kings
               && state.knights == subject.knights
               && state.archers == subject.archers
               && state.medics == subject.medics
               && state.wizards == subject.wizards
               && state.shields == subject.shields;
    });
    return iter != list.end();
}


TEST_CASE("Swap test", "[OffspringTests]") {
    State::setUp();
    Piece *board[4][4] = {
            {w3 __ M3 s1},
            {m3 n2 __ A3},
            {a3 S2 __ k4},
            {K4 N3 a3 W3}
    };
    auto state = State(board, Side::White, Phase::Switch, 0, 0);
    vector<State> offsprings = state.getOffsprings();
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
}

TEST_CASE("Knight test", "[OffspringTests]") {
    State::setUp();
    Piece *board[4][4] = {
            {__ __ __ __},
            {__ n3 A3 __},
            {__ k2 N1 w1},
            {K4 N3 a1 s2}
    };
    auto state = State(board, Side::Black, Phase::Act, 0, 0);
    vector<State> offsprings = state.getOffsprings();
    CHECK((int) offsprings.size() == 8);

    Piece *board1[4][4] = {
            {__ __ __ __},
            {__ n3 A3 __},
            {__ k1 N1 w1},
            {K4 N3 __ s2}
    };
    CHECK(contains(offsprings, State(board1, Side::White, Phase::Switch, 0, 0)));

    Piece *board2[4][4] = {
            {__ __ __ __},
            {__ n3 A3 __},
            {__ k2 N1 __},
            {K4 N3 __ s2}
    };
    CHECK(contains(offsprings, State(board2, Side::White, Phase::Switch, 0, 0)));

    Piece *board3[4][4] = {
            {__ __ __ __},
            {__ n3 A3 __},
            {__ k1 N1 __},
            {K4 N3 a1 s2}
    };
    CHECK(contains(offsprings, State(board3, Side::White, Phase::Switch, 0, 0)));

    Piece *board4[4][4] = {
            {__ __ __ __},
            {__ n3 A3 __},
            {__ k1 N1 w1},
            {K4 N3 a1 s2}
    };
    CHECK(contains(offsprings, State(board4, Side::White, Phase::Switch, 0, 0)));

    Piece *board5[4][4] = {
            {__ __ __ __},
            {__ n3 A3 __},
            {__ k2 N1 w1},
            {K4 N3 __ s2}
    };
    CHECK(contains(offsprings, State(board5, Side::White, Phase::Switch, 0, 0)));

    Piece *board6[4][4] = {
            {__ __ __ __},
            {__ n3 A3 __},
            {__ k2 N1 __},
            {K4 N3 a1 s2}
    };
    CHECK(contains(offsprings, State(board6, Side::White, Phase::Switch, 0, 0)));

    Piece *board7[4][4] = {
            {__ __ __ __},
            {__ n2 A3 __},
            {__ k2 N1 w1},
            {K4 N3 a1 s2}
    };
    CHECK(contains(offsprings, State(board7, Side::White, Phase::Switch, 0, 0)));

    Piece *board8[4][4] = {
            {__ __ __ __},
            {__ n3 A3 __},
            {__ k2 N1 w1},
            {K4 N3 a1 s2}
    };
    CHECK(contains(offsprings, State(board8, Side::White, Phase::Switch, 1, 0)));
}

TEST_CASE("Archer test", "[OffspringTests]") {
    State::setUp();
    Piece *board[4][4] = {
            {n2 k4 __ m3},
            {__ s1 __ __},
            {n2 A3 __ w1},
            {M1 K4 N3 A2}
    };
    auto state = State(board, Side::Black, Phase::Act, 0, 0);
    vector<State> offsprings = state.getOffsprings();
    CHECK((int) offsprings.size() == 5);

    Piece *board1[4][4] = {
            {n2 k4 __ m3},
            {__ s1 __ __},
            {n1 A3 __ w1},
            {M1 K4 N3 A2}
    };
    CHECK(contains(offsprings, State(board1, Side::White, Phase::Switch, 0, 0)));

    Piece *board2[4][4] = {
            {n2 k4 __ m3},
            {__ __ __ __},
            {n2 A3 __ w1},
            {M1 K4 N3 A2}
    };
    CHECK(contains(offsprings, State(board2, Side::White, Phase::Switch, 0, 0)));

    Piece *board3[4][4] = {
            {n2 k4 __ m3},
            {__ s1 __ __},
            {n2 A3 __ __},
            {M1 K4 N3 A2}
    };
    CHECK(contains(offsprings, State(board3, Side::White, Phase::Switch, 0, 0)));

    Piece *board4[4][4] = {
            {n2 k4 __ m2},
            {__ s1 __ __},
            {n2 A3 __ w1},
            {M1 K4 N3 A2}
    };
    CHECK(contains(offsprings, State(board4, Side::White, Phase::Switch, 0, 0)));

    Piece *board5[4][4] = {
            {n2 k4 __ m3},
            {__ s1 __ __},
            {n2 A3 __ w1},
            {M1 K4 N3 A2}
    };
    CHECK(contains(offsprings, State(board5, Side::White, Phase::Switch, 1, 0)));
}

TEST_CASE("Medic test", "[OffspringTests]") {
    State::setUp();
    Piece *board[4][4] = {
            {__ __ __ k3},
            {__ N3 __ w1},
            {A1 M3 N1 __},
            {__ K3 __ __}
    };
    auto state = State(board, Side::Black, Phase::Act, 0, 0);
    vector<State> offsprings = state.getOffsprings();
    CHECK((int) offsprings.size() == 8);

    Piece *board1[4][4] = {
            {__ __ __ k3},
            {__ N3 __ w1},
            {A2 M3 N1 __},
            {__ K3 __ __}
    };
    CHECK(contains(offsprings, State(board1, Side::White, Phase::Switch, 0, 0)));

    Piece *board2[4][4] = {
            {__ __ __ k3},
            {__ N3 __ w1},
            {A1 M3 N1 __},
            {__ K4 __ __}
    };
    CHECK(contains(offsprings, State(board2, Side::White, Phase::Switch, 0, 0)));

    Piece *board3[4][4] = {
            {__ __ __ k3},
            {__ N3 __ w1},
            {A1 M3 N2 __},
            {__ K3 __ __}
    };
    CHECK(contains(offsprings, State(board3, Side::White, Phase::Switch, 0, 0)));

    Piece *board4[4][4] = {
            {__ __ __ k3},
            {__ N3 __ w1},
            {A2 M3 N1 __},
            {__ K4 __ __}
    };
    CHECK(contains(offsprings, State(board4, Side::White, Phase::Switch, 0, 0)));

    Piece *board5[4][4] = {
            {__ __ __ k3},
            {__ N3 __ w1},
            {A2 M3 N2 __},
            {__ K3 __ __}
    };
    CHECK(contains(offsprings, State(board5, Side::White, Phase::Switch, 0, 0)));

    Piece *board6[4][4] = {
            {__ __ __ k3},
            {__ N3 __ w1},
            {A1 M3 N2 __},
            {__ K4 __ __}
    };
    CHECK(contains(offsprings, State(board6, Side::White, Phase::Switch, 0, 0)));

    Piece *board7[4][4] = {
            {__ __ __ k3},
            {__ N3 __ w1},
            {A2 M3 N2 __},
            {__ K4 __ __}
    };
    CHECK(contains(offsprings, State(board7, Side::White, Phase::Switch, 0, 0)));

    Piece *board8[4][4] = {
            {__ __ __ k3},
            {__ N3 __ w1},
            {A1 M3 N1 __},
            {__ K3 __ __}
    };
    CHECK(contains(offsprings, State(board8, Side::White, Phase::Switch, 1, 0)));
}

TEST_CASE("Wizard test", "[OffspringTests]") {
    State::setUp();
    Piece *board[4][4] = {
            {n2 k4 __ N3},
            {__ s1 __ __},
            {n2 M3 __ w1},
            {W1 K4 N3 S2}
    };
    auto state = State(board, Side::Black, Phase::Act, 0, 0);
    vector<State> offsprings = state.getOffsprings();
    CHECK((int) offsprings.size() == 6);

    Piece *board1[4][4] = {
            {n2 k4 __ N3},
            {__ s1 __ __},
            {n2 M3 __ w1},
            {K4 W1 N3 S2}
    };
    CHECK(contains(offsprings, State(board1, Side::White, Phase::Switch, 0, 0)));

    Piece *board2[4][4] = {
            {n2 k4 __ N3},
            {__ s1 __ __},
            {n2 W1 __ w1},
            {M3 K4 N3 S2}
    };
    CHECK(contains(offsprings, State(board2, Side::White, Phase::Switch, 0, 0)));

    Piece *board3[4][4] = {
            {n2 k4 __ N3},
            {__ s1 __ __},
            {n2 M3 __ w1},
            {N3 K4 W1 S2}
    };
    CHECK(contains(offsprings, State(board3, Side::White, Phase::Switch, 0, 0)));

    Piece *board4[4][4] = {
            {n2 k4 __ N3},
            {__ s1 __ __},
            {n2 M3 __ w1},
            {S2 K4 N3 W1}
    };
    CHECK(contains(offsprings, State(board4, Side::White, Phase::Switch, 0, 0)));

    Piece *board5[4][4] = {
            {n2 k4 __ W1},
            {__ s1 __ __},
            {n2 M3 __ w1},
            {N3 K4 N3 S2}
    };
    CHECK(contains(offsprings, State(board5, Side::White, Phase::Switch, 0, 0)));

    Piece *board6[4][4] = {
            {n2 k4 __ N3},
            {__ s1 __ __},
            {n2 M3 __ w1},
            {W1 K4 N3 S2}
    };
    CHECK(contains(offsprings, State(board6, Side::White, Phase::Switch, 1, 0)));
}
