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
