#include <iostream>
#include <fstream>
#include <string>
#include "L4.h"
#include "L3.h"

void test_l4_packets(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::cout << "=== Testing L4 Packets ===" << std::endl;

    /* define parameters for proccess_packet */
    open_port_vec open_ports;
    open_port port1 = open_port(2000, 1000);
    open_port port2 = open_port(4000, 3000);
    uint8_t ip[IP_V4_SIZE] = {192,168,10,0};
    uint8_t mask = 5;
    open_ports.push_back(port1);
    open_ports.push_back(port2);
    memory_dest dst;

    while (std::getline(file, line)) {
        try {
            l4_packet packet(line);
            std::string out;
            packet.as_string(out);

            if (packet.proccess_packet(open_ports, ip, mask, dst)) {
            	std::cout << "Parsed L4 Packet: " << out << std::endl;
            }
            else {
            	std::cout << "Thrown L4 Packet: " << out << std::endl;
            }
        } catch (...) {
            std::cerr << "Failed to parse L4 packet: " << line << std::endl;
        }
    }
    std::cout << std::endl;
}

void test_l3_packets(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::cout << "=== Testing L3 Packets ===" << std::endl;

    /* define parameters for proccess_packet */
   open_port_vec open_ports;
   open_port port1 = open_port(763, 4413);
   open_port port2 = open_port(2000, 2500); // test port
   uint8_t ip[IP_V4_SIZE] = {192,168,10,0};
   uint8_t mask = 20;
   open_ports.push_back(port1);
   open_ports.push_back(port2);
   memory_dest dst;

   std::string p = "4.52.123.8|4.52.123.6|23|1036|2500|2000|0|"
		   "dd 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"
		   "00 00 00 00 00 00 00 00 00 00 00";
   l3_packet packet(p);
   std::string out;
   packet.as_string(out);
   if (packet.proccess_packet(open_ports, ip, mask, dst)) {
		std::cout << "Parsed L3 Packet: " << out << std::endl;
   }
   else {
		std::cout << "Thrown L3 Packet: " << out << std::endl;
   }

 /*   while (std::getline(file, line)) {
        try {
            l3_packet packet(line);
            std::string out;
			packet.as_string(out);

			if (packet.proccess_packet(open_ports, ip, mask, dst)) {
				std::cout << "Parsed L3 Packet: " << out << std::endl;
			}
			else {
				std::cout << "Thrown L3 Packet: " << out << std::endl;
			}
        } catch (...) {
            std::cerr << "Failed to parse L3 packet: " << line << std::endl;
        }
    }
    std::cout << std::endl;*/
}

int main() {
	std::string packet_path;
	packet_path = "/media/sf_shared/hw_6/test_students/test0_packets.in";
    // test_l4_packets(packet_path);
    packet_path = "/media/sf_shared/hw_6/test_students/test1_packets.in";
    test_l3_packets(packet_path);
    return 0;
}

