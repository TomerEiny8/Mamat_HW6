#include <iostream>
#include <fstream>
#include <string>
#include "L4.h"
#include "L3.h"
#include "L2.h"

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

   while (std::getline(file, line)) {
	   l3_packet packet(line);
	   std::string out;
	   packet.as_string(out);

	   if (packet.proccess_packet(open_ports, ip, mask, dst)) {
		   if (dst == RQ)
			   std::cout << "Parsed L3 Packet into RQ: " << out << std::endl;
		   if (dst == TQ)
			   std::cout << "Parsed L3 Packet into TQ: " << out << std::endl;
		   if (dst == LOCAL_DRAM)
			   std::cout << "Parsed L3 Packet into LOCAL_DRAM: " << out << std::endl;
	   }
	   else {
		   std::cout << "Thrown L3 Packet: " << out << std::endl;
	   }
   }
	std::cout << std::endl;
}

void test_l2_packets(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::cout << "=== Testing L2 Packets ===" << std::endl;

    /* define parameters for proccess_packet */
    open_port_vec open_ports;
    open_port port1 = open_port(2033, 1687);
    open_port port2 = open_port(2080, 1587);
    uint8_t mac[MAC_SIZE] = {0x12,0x34,0x56,0x78,0xab,0xcd};
    uint8_t ip[IP_V4_SIZE] = {192,168,10,0};
    uint8_t mask = 20;
    open_ports.push_back(port1);
    open_ports.push_back(port2);
    memory_dest dst;

    while (std::getline(file, line)) {
		l2_packet packet(line);
		std::string out;
		packet.as_string(out);

		if (packet.validate_packet(open_ports, ip, mask, mac)) {
			std::cout << "Valid L2 Packet: " << out << std::endl;
			if (packet.proccess_packet(open_ports, ip, mask, dst)) {
				std::cout << "Parsed L2 Packet into " << dst << std::endl;
			}
			else {
				std::cout << "Invalid base L3_packet, THROWN! " << std::endl;
			}
		} else {
		std::cerr << "Invalid L2 packet: " << line << std::endl;
		}
    }
    std::cout << std::endl;
}

void test_l2_single_packet() {
    std::string line;
    std::cout << "=== Testing L2 SINGLE Packet ===" << std::endl;

    /* define parameters for proccess_packet */
    open_port_vec open_ports;
    open_port port1 = open_port(2033, 1687);
    open_port port2 = open_port(2080, 1587);
    // uint8_t mac[MAC_SIZE] = {0xc2,0x02,0x01,0x90,0x10,0x02}; // mac for 1st line
    uint8_t mac[MAC_SIZE] = {0x12,0x34,0x56,0x78,0xab,0xcd};	// mac for 2nd line
    uint8_t ip[IP_V4_SIZE] = {192,168,10,0};
    uint8_t mask = 20;
    open_ports.push_back(port1);
    open_ports.push_back(port2);
    memory_dest dst;

/*    line = "A1:12:57:9F:00:01|C2:02:01:90:10:02"
    		"|4.52.123.8|4.52.123.6|23|1036|2500|2000|0|"
    		"DD 00 00 00 00 00 00 00 00 00 00 00 "
    		"00 00 00 00 00 00 00 00 00 00 00 00 "
    		"00 00 00 00 00 00 00 00 00 00 00 00|1963";*/

    line = "45:60:6b:d9:15:8d|12:34:56:78:ab:cd|102.52.229.159|"
    		"172.131.17.206|4|5467|12|180|12|"
    		"43 05 16 cd 47 b4 bb eb af f8 41 "
    		"dd 8c a0 7f 88 c1 18 93 75 ba c3 "
    		"ae bb 35 3a 37 26 7a 6a a7 7d|6882";

	l2_packet packet(line);
	std::string out;
	packet.as_string(out);
	std::cout << "Test started" << std::endl;
	if (packet.validate_packet(open_ports, ip, mask, mac)) {
		std::cout << "Valid L2 Packet: " << out << std::endl;
		if (packet.proccess_packet(open_ports, ip, mask, dst)) {
			std::cout << "Parsed L2 Packet into " << dst <<  std::endl;
		}
		else {
			std::cout << "Invalid base L3_packet - THROWN! " << std::endl;
		}
	} else {
	std::cerr << "Invalid L2 packet: " << line << std::endl;
	}
    std::cout << std::endl;
}

int main() {
	std::string packet_path;
	packet_path = "/media/sf_shared/hw_6/test_students/test0_packets.in";
    test_l4_packets(packet_path);
    packet_path = "/media/sf_shared/hw_6/test_students/test1_packets.in";
    test_l3_packets(packet_path);
    packet_path = "/media/sf_shared/hw_6/test_students/test2_packets.in";
    test_l2_packets(packet_path);
    // test_l2_single_packet();
    return 0;
}
