#pragma once

#include <bitset>
#include <iostream>
#include <string>

#include "Common.h"
#include "LookupTables.h"
#include "State.h"

using std::string;

string cellToString(uint8 cell);

string displayAct(int move);

string displaySwap(uint8 move);

void printArcherPatterns();
