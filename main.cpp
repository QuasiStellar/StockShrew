#include <iostream>
#include <chrono>
#include <cstdlib>
#include <map>
#include "src/Piece.h"
#include "src/State.h"
#include "src/Phase.h"
#include "src/Console.h"
#include "src/LookupTables.h"

using std::vector, std::cout, std::endl, std::chrono::steady_clock, std::chrono::duration_cast, std::chrono::microseconds, std::string, std::multimap;

State fromInput(char *argv[]) {
    Piece *board[4][4] = {
            {p(argv[2]),  p(argv[3]),  p(argv[4]),  p(argv[5])},
            {p(argv[6]),  p(argv[7]),  p(argv[8]),  p(argv[9])},
            {p(argv[10]), p(argv[11]), p(argv[12]), p(argv[13])},
            {p(argv[14]), p(argv[15]), p(argv[16]), p(argv[17])}
    };
    Side side = argv[18][0] == 'b' || argv[18][0] == 'B' ? Side::Black : Side::White;
    Phase phase = argv[19][0] == 's' || argv[19][0] == 'S' ? Phase::Swap : Phase::Act;
    uint8 blackSkips = strtol(argv[20], nullptr, 10);
    uint8 whiteSkips = strtol(argv[21], nullptr, 10);
    return {board, side, phase, blackSkips, whiteSkips};
}

string getInfo() {
    return "_______________\n"
           "StockShrew v0.4\n"
           "https://github.com/QuasiStellar/StockShrew\n"
           "\n"
           "by QuasiStellar\n"
           "https://quasistellar.xyz\n"
           "\n"
           "StockShrew is a bitboard-based evaluation engine for the game Feud by BearWaves\n"
           "https://www.bearwaves.com/\n"
           "\n"
           "To analise a position run this program from a terminal with the following arguments:\n"
           "- depth of the search\n"
           "- all pieces on the board row by row from top to bottom from left to right\n"
           "each piece consists of a letter (type) and number (hp), black pieces use capitalised letters\n"
           "missing pieces are marked as __"
           "- side to play (b for black, w for white)\n"
           "- phase of the move (s for swap, a for act)\n"
           "- skips black has done in a row\n"
           "- skips white has done in a row\n"
           "\n"
           "For example in order to analise one of the common starting positions up to depth 8 you can use these arguments:\n"
           "8 a3 m3 a3 k4 w3 n3 n3 s4 S3 N3 N3 W3 K4 A3 M3 A3 b s 0 0\n"
           "\n"
           "Add parameter \"list\" at the end to list all possible moves along with their evaluations instead of doing the normal search.\n";
}

int main(int argc, char *argv[]) {

    if (argc == 1) {
        cout << getInfo() << endl;
        return 0;
    }

    uint8 depth = strtol(argv[1], nullptr, 10);
    State root = fromInput(argv);
    State::resetGlobals();
    setUp();

    Strategy strategy = Strategy::AlphaBeta;

    if (argc == 23 && argv[22]) {
        vector<pair<State, string>> variants = root.getOffsprings();
        multimap<float, string> evaluation_map = multimap<float, string>();
        for (auto variant: variants) {
            evaluation_map.insert(pair<float, string>(
                    variant.first.search(depth - 1, strategy),
                    variant.second)
            );
        }
        for (auto const &[eval, move]: evaluation_map)
            cout << move << " : " << eval << endl;
        return 0;
    }

    steady_clock::time_point begin = steady_clock::now();
    cout << "Search depth: " << (int) depth << endl;
    cout << "Doing the search... This may take a while..." << endl;
    float eval = root.search(depth, strategy);
    cout << "Position evaluation: " << eval << endl;
    cout << State::stateCount << " states checked" << endl;
    steady_clock::time_point end = steady_clock::now();
    cout << (double) duration_cast<microseconds>(end - begin).count() / 1000000.0 << " seconds elapsed" << endl;
}
