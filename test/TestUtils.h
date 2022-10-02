#pragma once

#include "../src/State.h"

using std::vector;

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
