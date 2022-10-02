#include <iostream>
#include <chrono>
#include <cstdlib>
#include "src/Piece.h"
#include "src/State.h"
#include "src/Phase.h"
#include "src/Console.h"
#include "src/LookupTables.h"

using std::vector, std::cout, std::endl, std::chrono::steady_clock, std::chrono::duration_cast, std::chrono::microseconds;

State fromInput(char *argv[]) {
    Piece *board[4][4] = {
            {p(argv[2]),  p(argv[3]),  p(argv[4]),  p(argv[5])},
            {p(argv[6]),  p(argv[7]),  p(argv[8]),  p(argv[9])},
            {p(argv[10]), p(argv[11]), p(argv[12]), p(argv[13])},
            {p(argv[14]), p(argv[15]), p(argv[16]), p(argv[17])}
    };
    Side side = argv[18][0] == 'b' || argv[18][0] == 'B' ? Side::Black : Side::White;
    Phase phase = argv[19][0] == 's' || argv[19][0] == 'S' ? Phase::Switch : Phase::Act;
    byte blackSkips = strtol(argv[20], nullptr, 10);
    byte whiteSkips = strtol(argv[21], nullptr, 10);
    return {board, side, phase, blackSkips, whiteSkips};
}

void printInfo() {
    cout << "_______________" << endl
         << "StockShrew v0.2" << endl
         << "https://github.com/QuasiStellar/StockShrew" << endl
         << endl
         << "by QuasiStellar" << endl
         << "https://quasistellar.xyz" << endl
         << endl
         << "StockShrew is bitboard-based an evaluation engine for the game Feud by BearWaves" << endl
         << "https://www.bearwaves.com/" << endl
         << endl
         << "To analise a position run this program from a terminal with the following arguments:" << endl
         << "- depth of the search" << endl
         << "- all pieces on the board row by row from top to bottom from left to right" << endl
         << "each piece consists of a letter (type) and number (hp), black pieces use capitalized letters" << endl
         << "- side to play (b for black, w for white)" << endl
         << "- phase of the move (s for switch, a for act)" << endl
         << "- skips black has done in a row" << endl
         << "- skips white has done in a row" << endl
         << endl
         << "For example in order to analise one of the common starting positions up to depth 8 you can use these arguments:" << endl
         << "8 a3 m3 a3 k4 w3 n3 n3 s3 S4 N3 N3 W3 K4 A3 M3 A3 b s 0 0" << endl
         << endl
         << "Add parameter \"list\" at the end to list all possible moves along with their evaluations instead of doing the normal search." << endl
         << endl;
}

int main(int argc, char *argv[]) {

    if (argc == 1) {
        printInfo();
        return 0;
    }

    byte depth = strtol(argv[1], nullptr, 10);
    State root = fromInput(argv);
    setUp();

    if (argc == 23 && argv[22]) {
        cout << "Move listings are a work in progress..." << endl;
    }

    steady_clock::time_point begin = steady_clock::now();
    cout << "Search depth: " << (int) depth << endl;
    float eval = root.search(depth, Strategy::AlphaBeta);
    cout << "Position evaluation: " << eval << endl;
    cout << State::stateCount << " states checked" << endl;
    steady_clock::time_point end = steady_clock::now();
    cout << duration_cast<microseconds>(end - begin).count() / 1000000.0 << " seconds elapsed" << endl;
}
