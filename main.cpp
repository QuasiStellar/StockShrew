#include <iostream>
#include <bitset>
#include <chrono>
#include "src/Piece.h"
#include "src/State.h"
#include "src/Phase.h"
#include "src/Abbreviations.h"

using std::vector, std::cout, std::endl, std::chrono::steady_clock, std::chrono::duration_cast, std::chrono::microseconds;

int main() {
    State::setUp();
    Piece *board[4][4] = {
            {a3 m3 a3 k4},
            {w3 n3 n3 s3},
            {S4 N3 N3 W3},
            {K4 A3 M3 A3}
    };
    auto state = State(board, Side::Black, Phase::Switch, 0, 0);
    steady_clock::time_point begin = steady_clock::now();
    cout << "Position evaluation: " << state.search(8) << endl;
    cout << State::stateCount << " states checked" << endl;
    steady_clock::time_point end = steady_clock::now();
    cout << duration_cast<microseconds>(end - begin).count() / 1000000.0 << " seconds elapsed" << endl;
}
