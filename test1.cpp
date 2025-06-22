#include "L3.h"
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
}

void test_l3_as_string() {
    std::string raw_data = "192.168.1.1|10.0.0.1|FF:FF:FF:FF:FF:FF|5|123|"  // L3 fields
                           "0|0|0|0|0|0|123|456|2|1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31";  // L4 fields
    l3_packet pkt(raw_data);

    std::string str;
    bool result = pkt.as_string(str);

    std::cout << "L3 as_string output: " << str << std::endl;
    assert(result);
}

int main() {
    test_l4_as_string();
    test_l3_as_string();
    std::cout << "All as_string tests passed." << std::endl;
    return 0;
}
