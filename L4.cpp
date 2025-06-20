/*
 * l4.cpp
 *
 *  Created on: 19 Jun 2025
 *      Author: mamat
 */

#include "L4.h"

/**
 * @fn l4_packet
 * @brief Constructor from given string
 *
 * @param [in] raw_data - string containing data formated as follows:
 * 		src_port(2 Bytes)|dst_port(2 Bytes)|addrs(4 Byes)|data(32 Bytes)
 *
 * @return new L4 packet
 */
l4_packet::l4_packet(const std::string& raw_data) {
	std::string tmp, byte_str;

	tmp = extract_between_delimiters(raw_data, '|', 0, 0);
	this->src_prt =  static_cast<unsigned short>(std::stoul(tmp));

	tmp = extract_between_delimiters(raw_data, '|', 1, 1);
	this->dst_prt =  static_cast<unsigned short>(std::stoul(tmp));

	tmp = extract_between_delimiters(raw_data, '|', 2, 2);
	this->addrs =  static_cast<unsigned int>(std::stoul(tmp));

	tmp = extract_between_delimiters(raw_data, '|', 3, -1);
	for (int i = 0; i < PACKET_DATA_SIZE; i++) {
		byte_str = extract_between_delimiters(tmp, ' ', i, i);
		if (i == PACKET_DATA_SIZE-1) {
			byte_str =
				extract_between_delimiters(tmp, ' ', PACKET_DATA_SIZE-1, -1);
		}
		if (byte_str.empty()) {
			this->data[i] = 0;
		} else {
			this->data[i] =
				static_cast<unsigned short>(std::stoul(byte_str, nullptr, 16));

		}
	}
}

	/* Getters */
	unsigned short l4_packet::get_src_port() const { return this->src_prt; }
	unsigned short l4_packet::get_dst_port() const { return this->dst_prt; }
	unsigned int l4_packet::get_addrs() const { return this->addrs; }


	bool l4_packet::validate_packet(open_port_vec open_ports,
            uint8_t ip[IP_V4_SIZE],
            uint8_t mask,
            uint8_t mac[MAC_SIZE]) {return true;}

	bool l4_packet::proccess_packet(open_port_vec &open_ports,
								uint8_t ip[IP_V4_SIZE],
								uint8_t mask,
								memory_dest &dst) {return true;}

	bool l4_packet::as_string(std::string &packet) {return true;}
