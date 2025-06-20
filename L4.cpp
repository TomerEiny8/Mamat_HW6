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
	explicit l4_packet::l4_packet(const std::string& raw_data) {
		std::string tmp = extract_between_delimiters(raw_data, '|', 0, 1);
		this->src_port =  static_cast<uint16_t>(std::stoul(tmp));

		tmp = (extract_between_delimiters(raw_data, '|', 1, 2));
		this->dst_port =  static_cast<uint16_t>(std::stoul(tmp));

		tmp = (extract_between_delimiters(raw_data, '|', 2, 3));
		this->addrs =  static_cast<uint32_t>(std::stoul(tmp));

		// need to add casting for data, perhaps with a loop?

	}

	/* Getters */
	unsigned short l4_packet::get_src_port() const { return this->src_port; }
	unsigned short l4_packet::get_dst_port() const { return this->dst_port; }
	unsigned int l4_packet::get_addrs() const { return this->addrs; }
