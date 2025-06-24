/*
 * NIC.cpp
 *
 *  Created on: 24 Jun 2025
 *      Author: mamat
 */

#include "NIC_sim.hpp"

/**
 * @fn nic_sim
 * @brief Constructor of the class.
 *
 * @param param_file - File name containing the NIC's parameters.
 *
 * @return New simulation object.
 */
nic_sim::nic_sim(std::string param_file) {
	std::ifstream file(param_file);
	std::string line, tmp, src_prt, dst_prt;
	open_port port;

	std::getline(file, line);
	extract_and_write(line, this->mac, HEX, ':');

	std::getline(file, line);
	tmp = extract_between_delimiters(line, '\'', 0, 0);
	extract_and_write(tmp, this->ip, DEC, ':');
	tmp = extract_between_delimiters(line, '\'', 1, -1);
	this->mask = static_cast<uint8_t>(std::stoul(tmp));

	while (std::getline(file, line)) {
		tmp = extract_between_delimiters(line, ':', 0, -1);
		src_prt = extract_between_delimiters(tmp, ',', 0, 0);
		dst_prt = extract_between_delimiters(tmp, ':', 1, -1);

		port = open_port(static_cast<uint16_t>(std::stoul(dst_prt)),
				static_cast<uint16_t>(std::stoul(src_prt)));
		this->open_ports.push_back(port);
	}
}

/**
 * @fn nic_flow
 * @brief Process and store to relevant location all packets in packet_file.
 *
 * @param packet_file - Name of file containing packets as strings.
 *
 * @return None.
 */
void nic_sim::nic_flow(std::string packet_file) {
	std::ifstream file(packet_file);
	std::string line, packet_string;
	memory_dest dst;

	while (std::getline(file, line)) {
		generic_packet* curr_packet = packet_factory(line);
		if (curr_packet->validate_packet
				(this->open_ports, this->ip, this->mask, this->mac)) {
			if (curr_packet->proccess_packet
					(this->open_ports, this->ip, this->mask, dst)) {
				if (dst == RQ) {
					curr_packet->as_string(packet_string);
					RQ.push_back(packet_string);
				}
				if (dst == TQ) {
					curr_packet->as_string(packet_string);
					RQ.push_back(packet_string);
				}
				// case LOCAL_DRAM handled in L4_packet::proccess_packet
			}
		}
	}
}

/**
 * @fn nic_print_results
 * @brief Prints all data stored in memory to stdout in the following format:
 *
 *        LOCAL DRAM:
 *        [src] [dst]: [data - DATA_ARR_SIZE bytes]
 *        [src] [dst]: [data - DATA_ARR_SIZE bytes]
 *        ...
 *
 *        RQ:
 *        [each packet in separate line]
 *
 *        TQ:
 *        [each packet in separate line]
 *
 * @return None.
 */
void nic_sim::nic_print_results() {
	std::string idx;
	std::cout << "LOCAL_DRAM:" << std::endl;

	for(open_port& port : open_ports) {
		std::cout << port.src_prt << " " << port.dst_prt << ": ";
		print_port_data(port);
	}

	std::cout << std::endl << "RQ:" << std::endl;
	for (std::string idx : RQ) {
		std::cout << idx << std::endl;
	}

	std::cout << std::endl << "TQ:" << std::endl;
	for (std::string idx : TQ) {
		std::cout << idx << std::endl;
	}
}

void print_port_data(open_port &port) {
	std::string prt_data;
	char byte_buf[3];	// "ff" + "\0"
	for(int i = 0; i < DATA_ARR_SIZE; i++){
	std::snprintf(byte_buf, sizeof(byte_buf), "%02x", port.data[i]);
	prt_data += byte_buf;
	if(i != DATA_ARR_SIZE -1) {
		prt_data += " ";
	}
	std::cout << prt_data << std::endl;
}
}

/**
 * @fn ~nic_sim
 * @brief Destructor of the class.
 *
 * @return None.
 */
nic_sim::~nic_sim() {

}

/**
 * @fn packet_factory
 * @brief 	Gets a string representing a packet, creates the corresponding
 *        	packet type, and returns a pointer to a generic_packet.
 *
 * @param 	packet - String representation of a packet.
 *
 * @return 	Pointer to a generic_packet object.
 * 			NULL if packet_type could not be detected.
 */
generic_packet* nic_sim::packet_factory(std::string &packet) {

	PACKET_TYPE packet_type = detect_packet_type(packet);

	switch (packet_type) {
	case L2: return new l2_packet::l2_packet(packet);
	case L3: return new l3_packet::l3_packet(packet);
	case L4: return new l4_packet::l4_packet(packet);
	default: return nullptr;
	}
}


/**
 * @fn detect_packet_type
 * @brief Gets a string representing a packet, returns the packet type.
 *
 * @param packet - String representation of a packet.
 *
 * @return PACKET_TYPE corresponding to the packet's type.
 */
PACKET_TYPE nic_sim::detect_packet_type(std::string &packet) {
	std::string first_field =
			generic_packet::extract_between_delimiters(packet, '|', 0, 0);
	for (char c : first_field) {
		if (c == ':') {
			return L2;
		}
		if (c == '.') {
			return L3;
		}
	}
	return L4;
}
