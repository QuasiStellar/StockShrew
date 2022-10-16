#pragma once

#include "../src/State.h"
#include "../src/StateDisplay.h"

using std::vector;

bool contains(const vector<State> &list, const State &subject) {
    auto iter = find_if(list.begin(), list.end(), [&](const State &state) {
        return displayState(state) == displayState(subject);
    });
    return iter != list.end();
}
