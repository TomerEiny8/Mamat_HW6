/*
 * L2.cpp
 *
 *  Created on: 23 Jun 2025
 *      Author: mamat
 */

#include "L2.h"

/**
 * @fn l2_packet
 * @brief Constructs an l2_packet from a given string
 *
 * @param [in] raw_data - string containing data formated as follows:
 * 		src_mac(6 Bytes)|dst_mac(6 Bytes)|"l3_packet"|CS_l2(4 Bytes)
 *
 * @return new L2 packet
 */
l2_packet::l2_packet(const std::string& raw_data)
	: l3_packet(extract_between_delimiters(raw_data, '|', l3_start, l3_end)) {
	std::string tmp, byte_str;
	l3_packet::as_string(tmp);

	tmp = extract_between_delimiters(raw_data, '|', 0, 0);
	extract_mac(tmp, this->src_mac);

	tmp = extract_between_delimiters(raw_data, '|', 1, 1);
	extract_mac(tmp, this->dst_mac);

	tmp = extract_between_delimiters(raw_data, '|', l3_end+1, -1);
	this->CS_l2 =  static_cast<unsigned int>(std::stoul(tmp));
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
bool l2_packet::validate_packet(open_port_vec open_ports,
								uint8_t ip[IP_V4_SIZE],
								uint8_t mask,
								uint8_t mac[MAC_SIZE]) {

	if (!l3_packet::validate_packet(open_ports, ip, mask, mac)) {
		std::cout << "Failed due to l3 validation error! " << std::endl;
		return false;
	}

	for (int i = 0; i < MAC_SIZE; i++) {
		if (this->dst_mac[i] != mac[i]){
			std::cout << "Failed due to MAC mismatch! " << std::endl;
			return false;
		}
	}
	unsigned int sum = calc_sum();
	if(this->CS_l2 != sum){
		std::cout << "Failed due to CS_l2 mismatch! " << std::endl;
		std::cout << "CS_l2 = " << this->CS_l2 << " sum = " << sum << std::endl;
		return false;
	}
	return true;
}

/**
 * @fn proccess_packet
 * @brief Modify the packet and return the memory location it should be
 *        stored in. In the case of local DRAM, the function will store
 *        the packet as a string in the relevant 'open_port' struct.
 *
 * @param [in] open_ports - Vector containidata[i]ng all the NIC's open ports.
 * @param [in] ip - NIC's IP address.
 * @param [in] mask - NIC's mask; together with the IP, determines the NIC's
 *        local net.
 * @param [out] dst - Reference to enum that indicate the memory space where
 *        the packet should be stored:
 *         LOCAL_DRAM - the function stored it to the currect struct.
 *         RQ - Should be stored as a string in RQ.
 *         TQ - Should be stored as string in TQ.
 *
 * @return l3_packet::proccess_packet(...) return value.
 */
bool l2_packet::proccess_packet(open_port_vec &open_ports,
								uint8_t ip[IP_V4_SIZE],
								uint8_t mask,
								memory_dest &dst) {

	/* @note - we can't call validate out of process since we don't have the
	 * mac address thus NIC.cpp has to first call validate and than process! */
	return l3_packet::proccess_packet(open_ports, ip, mask, dst);

}

/**
 * @fn as_string
 * @brief Convert the packet to string.
 *
 * @param [out] packet - Packet as a string.
 *
 * @return true always.
 */
bool l2_packet::as_string(std::string &packet){

	for(int i = 0; i < MAC_SIZE; i++){
		if(i != IP_V4_SIZE -1)
			packet += std::to_string(this->src_mac[i]) + ":";
		else
			packet += std::to_string(this->src_mac[i]) + "|";
	}

	for(int i = 0; i < MAC_SIZE; i++){
		if(i != IP_V4_SIZE -1)
			packet += std::to_string(this->dst_mac[i]) + ":";
		else
			packet += std::to_string(this->dst_mac[i]) + "|";
	}

	std::string l3_str;
	l3_packet::as_string(l3_str);
	packet += l3_str + "|" + std::to_string(this->CS_l2);

	return true;
}

/**
 * @fn calc_sum
 * @brief Calculates the byte-wise sum of all fields in the l2_packet.
 *
 * This includes each byte of `src_mac` and `dst_mac`
 * and the result of the base class (`l3_packet`) `calc_sum()` function.
 *
 * @return The total sum as an unsigned integer.
 */
unsigned int l2_packet::calc_sum() const {
	unsigned int nibble, sum = 0, CS_l3 =  l3_packet::get_CS_l3();

	for(int i = 0; i < MAC_SIZE; i++) {
		sum += (this->src_mac[i] + this->dst_mac[i]);
	}
	sum += l3_packet::calc_sum();

	for (int i = 0; i < 4; i++) {
		nibble = static_cast<unsigned int>((CS_l3 >> 8*i) & 0xFF);
		sum += nibble;
	}

	return sum;
}

/**
 * @fn extarct_ip
 * @brief extracts a 6-Byte MAC from its string representation
 *
 * This function parses a string-formatted MAC (e.g. "45:60:6b:d9:15:8d")
 * and fills a 6 Byte array with the corresponding numeric values
 *
 * @param [in] mac_str The string containing the MAC with ':' as a delimiter
 * @param [out] mac The array to store the parsed MAC address into
 *
 * @return None.
 */
void l2_packet::extract_mac(const std::string mac_str,
		uint8_t (&mac)[MAC_SIZE]) {
	std::string byte_str;
	for (int i = 0; i < MAC_SIZE; i++) {
		byte_str = extract_between_delimiters(mac_str, ':', i, i);
		if (i == MAC_SIZE-1) {
			byte_str = extract_between_delimiters
					(mac_str, ':', MAC_SIZE-1, -1);
		}
		if (byte_str.empty()) {
			byte_str = "0";
		}
		mac[i] = static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16));
	}
}
