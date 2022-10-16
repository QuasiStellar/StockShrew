#include "Display.h"

using std::bitset, std::cout, std::endl, std::reverse, std::replace, std::string, std::to_string;

string cellToString(byte cell) {
    switch (cell) {
        case 0:
            return "a1";
        case 1:
            return "b1";
        case 2:
            return "c1";
        case 3:
            return "d1";
        case 4:
            return "a2";
        case 5:
            return "b2";
        case 6:
            return "c2";
        case 7:
            return "d2";
        case 8:
            return "a3";
        case 9:
            return "b3";
        case 10:
            return "c3";
        case 11:
            return "d3";
        case 12:
            return "a4";
        case 13:
            return "b4";
        case 14:
            return "c4";
        case 15:
            return "d4";
        default:
            return "???";
    }
}

string displayAct(int move) {
    vector<string> targets = vector<string>();
    string display = " ";
    switch (move & 0b11) {
        case 0:
            return "skip";
        case 1:
            return "wizard swap " + cellToString(move >> 4 & 0b1111) + " " + cellToString(move >> 8);
        case 2:
        case 3:
            for (int i = 0; i < 16; i++) {
                if (move >> 4 & 1 << i) {
                    targets.push_back(cellToString(i));
                }
            }
            for (const auto& target : targets) {
                display += target + " ";
            }
            return (move & 0b11) == 2 ? "damage" + display : "heal" + display;
        default:
            return "???";
    }
}

string displaySwap(byte move) {
    return "swap " + cellToString(move & 0b1111) + " " + cellToString(move >> 4);
}

void printArcherPatterns() {
    for (int i = 0; i < 15; i++) {
        for (int j = i; j < 16; j++) {
            for (int shield = 0; shield < 16; shield++) {
                if (shield == i || shield == j) continue;
                string display;
                for (int cell = 12; cell >= 0; cell++) {
                    if (cell == i || cell == j) {
                        display += "A ";
                    } else if (cell == shield) {
                        display += "S ";
                    } else if (i != j && archerTargets[(1 << i) + (1 << j)][shield] & 1 << cell) {
                        display += "O ";
                    } else if (i == j && archerTargets[1 << i][shield] & 1 << cell) {
                        display += "O ";
                    } else {
                        display += "_ ";
                    }
                    if (cell % 4 == 3) {
                        display += '\n';
                        cell -= 8;
                    }
                }
                cout << display << endl;
            }
        }
    }
}
