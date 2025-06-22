// test_l4_as_string.cpp
#include "L4.h"
#include <iostream>
#include <cassert>

void test_l4_as_string() {
    std::string raw_data = "0|0|0|0|0|0|123|456|2|1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31";
    l4_packet pkt(raw_data);

    std::string str;
    bool result = pkt.as_string(str);

    std::cout << "L4 as_string output: " << str << std::endl;
    assert(result);

    // אפשר לבדוק גם מול פלט צפוי אם רוצים לוודא תאימות מלאה
    std::string expected = "123|456|2|1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31";
    assert(str == expected);
}

int main() {
    test_l4_as_string();
    std::cout << "l4_packet::as_string test passed." << std::endl;
    return 0;
}
