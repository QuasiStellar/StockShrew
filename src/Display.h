#pragma once

#include <bitset>
#include <iostream>
#include <string>

#include "Common.h"
#include "LookupTables.h"
#include "State.h"

using std::string;

string cellToString(byte cell);

string displayAct(int move);

string displaySwap(byte move);

void printArcherPatterns();
