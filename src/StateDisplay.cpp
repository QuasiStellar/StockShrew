#include "StateDisplay.h"

string displayState(State state) {
    string display;
    char hp;
    char type;
    for (int i = 12; i >= 0; i++) {
        if (state.hp1 & 1 << i) {
            hp = '1';
        } else if (state.hp2 & 1 << i) {
            hp = '2';
        } else if (state.hp3 & 1 << i) {
            hp = '3';
        } else if (state.hp4 & 1 << i) {
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
        if (state.kings & 1 << i) {
            type = 'k';
        } else if (state.knights & 1 << i) {
            type = 'n';
        } else if (state.archers & 1 << i) {
            type = 'a';
        } else if (state.medics & 1 << i) {
            type = 'm';
        } else if (state.wizards & 1 << i) {
            type = 'w';
        } else if (state.shields & 1 << i) {
            type = 's';
        }
        if (state.black & 1 << i) {
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
    display += state.stateInfo & 0b1 ? "white " : "black ";
    display += state.stateInfo & 0b10 ? "act " : "swap ";
    display += to_string((int) ((state.stateInfo & 0b1100) >> 2));
    display += ' ';
    display += to_string((int) ((state.stateInfo & 0b110000) >> 4));
    display += '\n';
    return display;
}
