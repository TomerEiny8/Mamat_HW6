/*
 * l3.cpp
 *
 *  Created on: 21 Jun 2025
 *      Author: mamat
 */

#include "L3.h"

/**
 * @fn l3_packet
 * @brief Constructs an l3_packet from a given string
 *
 * @param [in] raw_data - string containing data formated as follows:
 * 		src_ip(4 Bytes)|dst_ip(4 Bytes)|TTL(4 Bytes)|CS_ls(4 Bytes)|"l4_packet"
 *
 * @return new L3 packet
 */
l3_packet::l3_packet(const std::string& raw_data)
	: l4_packet(extract_between_delimiters
			(raw_data, '|', l4_start, l4_end)) {
	std::string tmp, byte_str;

	tmp = extract_between_delimiters(raw_data, '|', 0, 0);
	extract_and_write(tmp, this->src_ip, DEC, '.');

	tmp = extract_between_delimiters(raw_data, '|', 1, 1);
	extract_and_write(tmp, this->dst_ip, DEC, '.');

	tmp = extract_between_delimiters(raw_data, '|', 2, 2);
	this->TTL =  static_cast<unsigned int>(std::stoul(tmp));

	tmp = extract_between_delimiters(raw_data, '|', 3, 3);
	this->CS_l3 =  static_cast<unsigned int>(std::stoul(tmp));
}

/* Getters */
unsigned int l3_packet::get_CS_l3() const { return this->CS_l3; }

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
bool l3_packet::validate_packet(open_port_vec open_ports,
								uint8_t ip[IP_V4_SIZE],
								uint8_t mask,
								uint8_t mac[MAC_SIZE]) {
	if (!l4_packet::validate_packet(open_ports, ip, mask, mac)) {
		return false;
	}

	//////////////////////////////////////////////////////////////
	if(static_cast<int>(this->TTL) <= 0){
		return false;
	}

	unsigned int sum = l3_packet::calc_sum();
	if(this->CS_l3 != sum){
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
 * @return true in success,
 * 			false in failure - dumping packet due to TTL or CS_l3 error.
 *
 */
bool l3_packet::proccess_packet(open_port_vec &open_ports,
								uint8_t ip[IP_V4_SIZE],
								uint8_t mask,
								memory_dest &dst) {

	// case 2.4 - ip_dst = NIC (without the mask)
	if (memcmp(this->dst_ip, ip, IP_V4_SIZE) == 0) {
		uint8_t dummy_mac[MAC_SIZE] = {0};
		if (!l4_packet::validate_packet(open_ports, ip, mask, dummy_mac)){
			return false;
		}
		return l4_packet::proccess_packet(open_ports, ip, mask, dst);
	}

	bool is_src_in_net = (ip_with_mask(mask, this->src_ip) ==
								ip_with_mask(mask, ip));

	bool is_dst_in_net = (ip_with_mask(mask, this->dst_ip) ==
								ip_with_mask(mask, ip));

	// case 2.5 - packet is in inside the network
	if(is_src_in_net && is_dst_in_net){
		return false;
	}

	this->TTL--;
	this->CS_l3--;
	if (this->TTL <= 0){
		return false;
	}

	// case 2.1 - Packet entering the network - with the mask
	if(!is_src_in_net && is_dst_in_net){
		dst = RQ;
	}

	// case 2.2 - Packet exiting the network - with the mask
	else if(is_src_in_net && !is_dst_in_net){
		int CS_l3_diff = ip_sum_diff(this->src_ip, ip);
		memcpy(this->src_ip, ip, IP_V4_SIZE);
		this->CS_l3 += CS_l3_diff;
		dst = TQ;
	}

	// case 2.3 - Packet is going through - with the mask
	else if(!is_src_in_net && !is_dst_in_net){
		dst = TQ;
	}

	return true;
}

int l3_packet::ip_sum_diff(uint8_t curr_ip[IP_V4_SIZE], uint8_t new_ip[IP_V4_SIZE]) {
	int diff = 0;
	for (int i = 0; i < IP_V4_SIZE; i++) {
		diff += (new_ip[i] - curr_ip[i]);
	}
	return diff;
}

/**
 * @fn as_string
 * @brief Convert the packet to string.
 *
 * @param [out] packet - Packet as a string, ready to be stored in RQ/TQ.
 *
 * @return true always.
 */
bool l3_packet::as_string(std::string &packet){
	packet = "";

	for(int i = 0; i < IP_V4_SIZE; i++){
		if(i != IP_V4_SIZE -1)
			packet += std::to_string(this->src_ip[i]) + ".";
		else
			packet += std::to_string(this->src_ip[i]) + "|";
	}

	for(int i = 0; i < IP_V4_SIZE; i++){
		if(i != IP_V4_SIZE -1)
			packet += std::to_string(this->dst_ip[i]) + ".";
		else
			packet += std::to_string(this->dst_ip[i]) + "|";
	}

	std::string l4_str;
	l4_packet::as_string(l4_str);
	packet += std::to_string(this->TTL) + "|" +
				  std::to_string(this->CS_l3) + "|" + l4_str;

	return true;
}

/**
 * @fn calc_sum
 * @brief Calculates the byte-wise sum of all fields in the l3_packet.
 *
 * This includes the `TTL` field, each byte of `src_ip` and `dst_ip`
 * and the result of the base class (`l4_packet`) `calc_sum()` function.
 *
 * @return The total sum as an unsigned integer.
 */
unsigned int l3_packet::calc_sum() const {
	unsigned int sum = 0;

	for(int i = 0; i<IP_V4_SIZE; i++) {
		sum += ((this->TTL >> i*8) & 0xFF);
		sum += (this->src_ip[i] + this->dst_ip[i]);
	}
	sum += l4_packet::calc_sum();

	return sum;
}

/**
 * @fn ip_with_mask
 * @brief return the ip with the mask number of left bits
 *
 * @param mask - number of left bits to be considered
 * @param ip - the number in the shape of an array, that we
 * return his left bits only
 *
 * @return uint32_t representation of the "mask" left-most bits in `ip`
 */
uint32_t l3_packet::ip_with_mask(uint8_t mask, uint8_t ip[IP_V4_SIZE]){
	if(mask == 0) return 0;
	if(mask >= (8*IP_V4_SIZE)) mask = (8*IP_V4_SIZE);

	uint32_t num = 0;

	for(int i = 0; i < IP_V4_SIZE; i++){
		num = (num << 8) | ip[i];
	}

	num >>= (8*IP_V4_SIZE) - mask;
	return num;
}
