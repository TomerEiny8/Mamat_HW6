#include <iostream>
#include <iomanip>
#include <string>
#include <cassert>
#include "L4.h"      // or "L4.h" if that’s your real name

int main() {
    const std::string raw =
      "1000|2000|0|"
      "00 11 22 33 44 55 66 77 "
      "88 99 aa bb cc dd ee ff "
      "00 11 22 33 44 55 66 77 "
      "88 99 aa bb cc dd ee ff";

    l4_packet p(raw);

    // check the header fields
    assert(p.get_src_port() == 1000);
    assert(p.get_dst_port() == 2000);
    assert(p.get_addrs()    == 0);

    // print them out
    std::cout << "src_port: " << p.get_src_port() << "\n";
    std::cout << "dst_port: " << p.get_dst_port() << "\n";
    std::cout << "addrs:    " << p.get_addrs()    << "\n\n";

    // now dump the 32‐byte payload in hex
    std::cout << "data: ";
    std::cout << std::hex << std::setfill('0');
    for (size_t i = 0; i < 32; ++i) {
        unsigned v = p.get_data_byte(i);
        std::cout << std::setw(2) << v
                  << (i + 1 < 32 ? " " : "\n");
    }

    std::cout << "\ndata check passed!\n";
    return 0;
}


