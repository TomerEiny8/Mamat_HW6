/*
 * l4.cpp
 *
 *  Created on: 19 Jun 2025
 *      Author: mamat
 */

#include "L4.h"

/**
 * @fn l4_packet
 * @brief Constructs an l4_packet from a given string
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
	extract_and_write(tmp, this->data, HEX, ' ');
}

/*
 * @fn validate_packet
 * @brief Check whether the packet is valid.
 * @param [in] open_ports - Vector containing all the NIC's open ports.
 * @param [in] ip - NIC's IP address.
 * @param [in] mask - NIC's mask; together with the IP,
 *             determines the NIC's local net.
 * @param [in] mac - NIC's MAC address.
 *
 * @return true if the packet is valid and ready for processing.
 *         false if the packet isn't valid and should be discarded.
 */
bool l4_packet::validate_packet(open_port_vec open_ports,
								uint8_t ip[IP_V4_SIZE],
								uint8_t mask,
								uint8_t mac[MAC_SIZE]) {
	bool is_src_pckt = false;
	bool is_dst_pckt = false;
	if(this->src_prt == this->dst_prt) {
		return false;
	}
	if(this->addrs >= (DATA_ARR_SIZE - PACKET_DATA_SIZE)){
		return false;
	}
	for(open_port& port : open_ports){
		if(this->dst_prt == port.dst_prt) {
			is_dst_pckt = true;
		}
	}
	for(open_port& port : open_ports){
		if(this->src_prt == port.src_prt) {
			is_src_pckt = true;
		}
	}
	if(is_src_pckt && is_dst_pckt){
		return true;
	}
	return false;
}

/**
 * @fn proccess_packet
 * @brief Modify the packet and return the memory location it should be
 *        stored in. In the case of local DRAM, the function will store
 *        the packet as a string in the relevant 'open_port' struct.
 *
 * @param [in] open_ports - Vector containing all the NIC's open ports.
 * @param [in] ip - NIC's IP address.
 * @param [in] mask - NIC's mask; together with the IP, determines the NIC's
 *        local net.
 * @param [out] dst - Reference to enum that indicate the memory space where
 *        the packet should be stored:
 *         LOCAL_DRAM - the function stored it to the currect struct.
 *         RQ - Should be stored as a string in RQ.
 *         TQ - Should be stored as string in TQ.
 *
 * @return true in success, false in case packet is invalid.
 */

bool l4_packet::proccess_packet(open_port_vec &open_ports,
								uint8_t ip[IP_V4_SIZE],
								uint8_t mask,
								memory_dest &dst) {

/*	// @note this check is probably unnecessary!
	uint8_t dummy_mac[MAC_SIZE] = {0};
	if(!l4_packet::validate_packet(open_ports, ip, mask, dummy_mac)){
		return false;
	}*/

	for(open_port& port : open_ports){
		if(this->dst_prt == port.dst_prt) {
			memcpy(&port.data[this->addrs], this->data, PACKET_DATA_SIZE);
			dst = LOCAL_DRAM;
		}
	}
	return true;
}

/**
 * @fn as_string
 * @brief Convert the packet to string.
 *
 * @param [out] packet - Packet as a string, ready to be stored in RQ/TQ.
 *
 * @return true always.
 */
bool l4_packet::as_string(std::string &packet){
	packet = std::to_string(this->src_prt) + "|" +
			 std::to_string(this->dst_prt) + "|" +
			 std::to_string(this->addrs) + "|";

	char byte_buf[3]; 	// "ff" + "\0"
	for(int i = 0; i < PACKET_DATA_SIZE; i++){
		std::snprintf(byte_buf, sizeof(byte_buf), "%02x", this->data[i]);
		packet += byte_buf;
		if(i != PACKET_DATA_SIZE -1) {
			packet += " ";
		}
	}
	return true;
}

/**
 * @fn calc_sum
 * @brief Calculates the byte-wise sum of all fields in the l4_packet.
 *
 * This includes the `addrs` field, both bytes of `src_prt` and `dst_prt`
 * and each byte of the `data` field.
 *
 * @return The total sum as an unsigned integer.
 */
unsigned int l4_packet::calc_sum() const {
	unsigned int prt_low, prt_high, sum = this->addrs;

	prt_low = static_cast<unsigned int>(this->src_prt & 0xFF);
	prt_high = static_cast<unsigned int>((this->src_prt >> 8) & 0xFF);
	sum += (prt_low + prt_high);

	prt_low = static_cast<unsigned int>(this->dst_prt & 0xFF);
	prt_high = static_cast<unsigned int>((this->dst_prt >> 8) & 0xFF);
	sum += (prt_low + prt_high);

	for (unsigned char data_byte : this->data) {
		sum += static_cast<unsigned int>(data_byte);
	}
	return sum;
}
