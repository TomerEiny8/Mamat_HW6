#include "L3.h"
#include "L4.h"
#include "common.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <cstring>

void make_ip(uint8_t ip[4], uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    ip[0] = a; ip[1] = b; ip[2] = c; ip[3] = d;
}

void test_l4_validate() {
    l4_packet pkt;
    pkt.port = 10;
    pkt.dst = 20;
    open_port_vec ports = {{10, 20}};
    make_ip(pkt.src_ip, 1, 2, 3, 4);
    make_ip(pkt.dst_ip, 5, 6, 7, 8);
    memset(pkt.mac, 0xFF, MAC_SIZE);
    assert(pkt.validate_packet(ports, pkt.src_ip, 24, pkt.mac));
    std::cout << "✅ test_l4_validate passed\n";
}

void test_l4_as_string() {
    l4_packet pkt;
    pkt.port = 5;
    pkt.dst = 7;
    std::string s;
    pkt.as_string(s);
    assert(!s.empty());
    std::cout << "✅ test_l4_as_string passed\n";
}

void test_l4_process_packet() {
    l4_packet pkt;
    pkt.port = 1;
    pkt.dst = 2;
    pkt.addrs = 0;
    memset(pkt.data, 0xAB, PACKET_DATA_SIZE);
    open_port_vec ports = {{1, 2}};
    int dst = -1;
    assert(pkt.process_packet(ports, pkt.src_ip, 24, dst));
    assert(dst == LOCAL_DRAM);
    std::cout << "✅ test_l4_process_packet passed\n";
}

void test_l3_validate() {
    l3_packet pkt;
    std::string str;
    pkt.as_string(str);
    pkt.CS_l3 = 0;
    for (char c : str)
        pkt.CS_l3 += static_cast<uint8_t>(c);
    assert(pkt.validate_packet({}, pkt.src_ip, 24, pkt.mac));
    std::cout << "✅ test_l3_validate passed\n";
}

void test_l3_as_string() {
    l3_packet pkt;
    std::string str;
    pkt.as_string(str);
    assert(!str.empty());
    std::cout << "✅ test_l3_as_string passed\n";
}

void test_l3_getters() {
    l3_packet pkt;
    make_ip(pkt.src_ip, 192, 168, 1, 1);
    make_ip(pkt.dst_ip, 10, 0, 0, 1);
    assert(pkt.get_src_ip() != 0);
    assert(pkt.get_dst_ip() != 0);
    std::cout << "✅ test_l3_getters passed\n";
}

void prepare_l3(l3_packet &pkt, uint8_t src[4], uint8_t dst[4], uint8_t my_ip[4], uint8_t ttl = 5) {
    memcpy(pkt.src_ip, src, 4);
    memcpy(pkt.dst_ip, dst, 4);
    memcpy(pkt.mac, "ABCD", 4);
    pkt.TTL = ttl;
    pkt.CS_l3 = 0;
    std::string str;
    pkt.as_string(str);
    for (char c : str) pkt.CS_l3 += static_cast<uint8_t>(c);
}

void test_l3_process_case_2_1() {
    l3_packet pkt;
    uint8_t src_ip[4] = {10, 0, 0, 2};
    uint8_t dst_ip[4] = {10, 0, 0, 1};
    uint8_t my_ip[4] = {10, 0, 0, 1};
    prepare_l3(pkt, src_ip, dst_ip, my_ip);
    std::vector<std::string> RQ;
    open_port_vec ports;
    int dst = -1;
    assert(pkt.process_packet(ports, my_ip, 24, dst, RQ));
    assert(dst == LOCAL_DRAM);
    assert(!RQ.empty());
    std::cout << "✅ test_l3_process_case_2_1 passed\n";
}

void test_l3_process_case_2_2() {
    l3_packet pkt;
    uint8_t src_ip[4] = {10, 0, 0, 2};
    uint8_t dst_ip[4] = {8, 8, 8, 8};
    uint8_t my_ip[4] = {10, 0, 0, 1};
    prepare_l3(pkt, src_ip, dst_ip, my_ip);
    std::vector<std::string> TQ;
    open_port_vec ports = {{1, 2}};
    int dst = -1;
    assert(pkt.process_packet(ports, my_ip, 24, dst, TQ));
    assert(dst == NIC_OUTPUT_QUEUE);
    assert(!TQ.empty());
    std::cout << "✅ test_l3_process_case_2_2 passed\n";
}

void test_l3_process_case_2_3() {
    l3_packet pkt;
    uint8_t src_ip[4] = {10, 0, 0, 2};
    uint8_t dst_ip[4] = {10, 0, 0, 3};
    uint8_t my_ip[4] = {10, 0, 0, 1};
    prepare_l3(pkt, src_ip, dst_ip, my_ip);
    pkt.TTL = 0;
    std::vector<std::string> Q;
    open_port_vec ports;
    int dst = -1;
    assert(!pkt.process_packet(ports, my_ip, 24, dst, Q));
    std::cout << "✅ test_l3_process_case_2_3 passed\n";
}

void test_l3_process_case_2_4() {
    l3_packet pkt;
    uint8_t src_ip[4] = {10, 0, 0, 2};
    uint8_t dst_ip[4] = {10, 0, 0, 1};
    uint8_t my_ip[4] = {10, 0, 0, 99};
    prepare_l3(pkt, src_ip, dst_ip, my_ip);
    std::vector<std::string> Q;
    open_port_vec ports = {{1, 2}};
    int dst = -1;
    assert(pkt.process_packet(ports, my_ip, 24, dst, Q));
    assert(dst == LOCAL_DRAM);
    std::cout << "✅ test_l3_process_case_2_4 passed\n";
}

void test_l3_process_case_2_5() {
    l3_packet pkt;
    uint8_t ip[4] = {10, 0, 0, 1};
    prepare_l3(pkt, ip, ip, ip);
    std::vector<std::string> Q;
    open_port_vec ports;
    int dst = -1;
    assert(!pkt.process_packet(ports, ip, 24, dst, Q));
    std::cout << "✅ test_l3_process_case_2_5 passed\n";
}

int main() {
    test_l4_validate();
    test_l4_as_string();
    test_l4_process_packet();
    test_l3_validate();
    test_l3_as_string();
    test_l3_getters();
    test_l3_process_case_2_1();
    test_l3_process_case_2_2();
    test_l3_process_case_2_3();
    test_l3_process_case_2_4();
    test_l3_process_case_2_5();

    std::cout << "\n✅ All function and case-based tests completed successfully!\n";
    return 0;
}
