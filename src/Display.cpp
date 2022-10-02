#include "Display.h"

using std::bitset, std::cout, std::endl, std::reverse, std::replace;

void displayBitmap(ushort bitmap) {
    auto line1 = bitset<4>(bitmap & 0b1111).to_string();
    auto line2 = bitset<4>((bitmap >> 4) & 0b1111).to_string();
    auto line3 = bitset<4>((bitmap >> 8) & 0b1111).to_string();
    auto line4 = bitset<4>((bitmap >> 12) & 0b1111).to_string();
    reverse(line1.begin(), line1.end());
    reverse(line2.begin(), line2.end());
    reverse(line3.begin(), line3.end());
    reverse(line4.begin(), line4.end());
    cout << line4 << endl << line3 << endl << line2 << endl << line1 << endl << endl;
}
