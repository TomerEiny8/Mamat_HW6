#include "L3.h"
#include "L4.h"
#include "common.hpp"
#include <iostream>
#include <cassert>

void test_l3_validate() {
    l3_packet p;
    // הגדר שדות בצורה ידנית לפי מבנה הפקטה לצורך בדיקה
    p.CS_l3 = 0;
    std::string str;
    p.as_string(str);
    // checksum פשוט
    for (char c : str) p.CS_l3 += static_cast<uint8_t>(c);
    assert(p.validate_packet({/*open_ports*/}, p.src_ip, 32, p.mac) == true);
    std::cout << "✅ l3::validate_packet passed.\n";
}

void test_l4_validate() {
    l4_packet p;
    p.port = 10;
    p.dst = 20;
    std::vector<open_port> open_ports = {{10, 20}};
    assert(p.validate_packet(open_ports, p.src_ip, 32, p.mac) == true);
    std::cout << "✅ l4::validate_packet passed.\n";
}

int main() {
    test_l3_validate();
    test_l4_validate();

    // כאן תוכל להוסיף בדיקות נוספות כמו process_packet וכו'
    return 0;
}
