#pragma once

#include "../src/State.h"

using std::vector;

bool contains(const vector<State> &list, const State &subject) {
    auto iter = find_if(list.begin(), list.end(), [&](const State &state) {
        return state.display() == subject.display();
    });
    return iter != list.end();
}
