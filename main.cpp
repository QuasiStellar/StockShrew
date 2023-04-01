#include <iostream>
#include <chrono>
#include <cstdlib>
#include <map>
#include "src/Piece.h"
#include "src/State.h"
#include "src/Phase.h"
#include "src/Console.h"
#include "src/LookupTables.h"
#include "boost/program_options.hpp"

namespace po = boost::program_options;
using std::vector, std::cout, std::endl, std::chrono::steady_clock, std::chrono::duration_cast, std::chrono::microseconds, std::string, std::multimap, std::stoi;

State fromInput(vector<string> params) {
    Piece *board[4][4] = {
            {p(params[0]),  p(params[1]),  p(params[2]),  p(params[3])},
            {p(params[4]),  p(params[5]),  p(params[6]),  p(params[7])},
            {p(params[8]),  p(params[9]),  p(params[10]), p(params[11])},
            {p(params[12]), p(params[13]), p(params[14]), p(params[15])}
    };
    Side side = params[16][0] == 'b' || params[16][0] == 'B' ? Side::Black : Side::White;
    Phase phase = params[17][0] == 's' || params[17][0] == 'S' ? Phase::Swap : Phase::Act;
    uint8 blackSkips = stoi(params[18]);
    uint8 whiteSkips = stoi(params[19]);
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
           "This program will analise the given position and output the best possible move.\n"
           "The accuracy depends on the search depth which must be specified via a parameter: \"-d8\"\n"
           "\n"
           "To analise a position you need to provide it in the following format:\n"
           "- all pieces on the board row by row from top to bottom from left to right\n"
           "each piece consists of a letter (type) and number (hp), black pieces use capitalised letters\n"
           "missing pieces are marked as __\n"
           "- side to play (b for black, w for white)\n"
           "- phase of the move (s for swap, a for act)\n"
           "- skips black has done in a row\n"
           "- skips white has done in a row\n"
           "\n"
           "Verbouse output including the total number of checked positions and execution time can be turned on with \"-v\"\n"
           "Every available move can be listed via \"-l\"\n"
           "Evaluation numbers can be added via \"-e\". Positive numbers signify an advantage for black, negative - for white.\n"
           "\n"
           "For example in order to analise one of the common starting positions up to depth 8 you can use these arguments:\n"
           "-vled8 a3 m3 a3 k4 w3 n3 n3 s4 S3 N3 N3 W3 K4 A3 M3 A3 b s 0 0\n";
}

int main(int argc, char *argv[]) {

    if (argc == 1) {
        cout << getInfo() << endl;
        return 0;
    }

    try {
        int depth;

        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce this help message")
            ("verbose,v", "enable verbosity: output will include logs as well as total number of positions checked and execution time")
            ("list,l", "rank every available move (some of the worst moves may be skipped on occasion)")
            ("eval,e", "show evaluation: positive - good for black, negative - good for white")
            ("depth,d", po::value<int>(&depth)->required(), "depth of the search: improves accuracy and greately increases execution time")
            ("game", po::value<vector<string>>(), "game")
        ;

        po::positional_options_description positional;
        positional.add("game", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).positional(positional).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << desc << "\n";
            return 0;
        }

        bool verbose = vm.contains("verbose");
        bool list = vm.contains("list");
        bool eval = vm.contains("eval");

        State root = fromInput(vm["game"].as<vector<string>>());
        State::resetGlobals();
        setUp();

        Strategy strategy = Strategy::AlphaBeta;

        if (list) {
            vector<pair<State, string>> variants = root.getOffsprings();
            multimap<int, string> evaluation_map = multimap<int, string>();
            for (auto variant: variants) {
                evaluation_map.insert(pair<int, string>(
                    variant.first.search(depth - 1, strategy),
                    variant.second)
                );
            }
            for (auto const &[evaluation, move]: evaluation_map)
                if (eval) {
                    cout << move << " : " << evaluation << endl;
                } else {
                    cout << move << endl;
                }
            return 0;
        }

        steady_clock::time_point begin;
        if (verbose) {
            begin = steady_clock::now();
            cout << "Search depth: " << depth << endl;
            cout << "Doing the search... This may take a while..." << endl;
        }
        pair<int, string> evaluationAndBestMove = root.searchForMove(depth);
        int evaluation = evaluationAndBestMove.first;
        string bestMove = evaluationAndBestMove.second;
        if (verbose) {
            if (eval) {
                cout << "Position evaluation: " << evaluation << endl;
            }
            cout << "Best move: " << bestMove << endl;
            cout << State::stateCount << " states checked" << endl;
            steady_clock::time_point end = steady_clock::now();
            cout << (double) duration_cast<microseconds>(end - begin).count() / 1000000.0 << " seconds elapsed" << endl;
        } else {
            if (eval) {
                cout << evaluation << endl;
            }
            cout << bestMove << endl;
        }
    } catch(std::exception& e) {
        cout << e.what() << "\n";
    }
}
