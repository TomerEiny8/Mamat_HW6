/*
 * l3.cpp
 *
 *  Created on: 21 Jun 2025
 *      Author: mamat
 */

#include "L3.h"
#include <cstring>


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
	: l4_packet(extract_between_delimiters(raw_data, '|', 4, -1)) {
	std::string tmp, byte_str;

	tmp = extract_between_delimiters(raw_data, '|', 0, 0);
	extract_ip(tmp, this->src_ip);

	tmp = extract_between_delimiters(raw_data, '|', 1, 1);
	extract_ip(tmp, this->dst_ip);

	tmp = extract_between_delimiters(raw_data, '|', 2, 2);
	this->TTL =  static_cast<unsigned int>(std::stoul(tmp));

	tmp = extract_between_delimiters(raw_data, '|', 3, 3);
	this->CS_l3 =  static_cast<unsigned int>(std::stoul(tmp));
}

/**
 * @fn extarct_ip
 * @brief extracts a 4-Byte IPv4 from its string representation
 *
 * This function parses a string-formatted IPv4 (e.g. "140.60.40.54")
 * and fills a 4Byte array with the corresponding numeric values
 *
 * @param [in] ip_str The string containing the IP with '.' as a delimiter
 * @param [out] ip The array to store the parsed IP address
 *
 * @return None.
 */
void l3_packet::extract_ip(const std::string ip_str, uint8_t (&ip)[IP_V4_SIZE]) {
	std::string byte_str;
	for (int i = 0; i < IP_V4_SIZE; i++) {
		byte_str = extract_between_delimiters(ip_str, '.', i, i);
		if (i == IP_V4_SIZE-1) {
			byte_str = extract_between_delimiters
					(ip_str, '.', IP_V4_SIZE-1, -1);
		}
		if (byte_str.empty()) {
			byte_str = "0";
		}
		ip[i] = static_cast<uint8_t>(std::stoul(byte_str));
	}
}

/* Getters */
const uint8_t* l3_packet::get_src_ip() const { return this->src_ip; }
const uint8_t* l3_packet::get_dst_ip() const { return this->dst_ip; }
unsigned int l3_packet::get_TTL() const { return this->TTL; }
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
	std::cout << "entered l3_packet::validate_packet" << std::endl;
	if(!l4_packet::validate_packet(open_ports, ip, mask, mac)){
		return false;
	}
	if(this->TTL <= 0){
		return false;
	}
	unsigned int sum = 0;
	std::string str;
	l3_packet::as_string(str);
	std::string packet_string = extract_between_delimiters(str,'|', 0);
	std::cout << "packet_string = " << packet_string << std::endl;
	std::cout << "sum =";
	for(char c : packet_string){
		std::cout << " + " << c;
		sum += static_cast<unsigned int>(c);
	}
	sum -= this->CS_l3;
	std::cout << std::endl << "sum = " << sum << " , CS_l3 = " << this->CS_l3 << std::endl;
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
	uint8_t dummy_mac[MAC_SIZE] = {0};
	if(!this->validate_packet(open_ports, ip, mask, dummy_mac)){
		return false;
	}

	// case 2.4 - ip_dst = NIC (without the mask)
	if (memcmp(this->dst_ip, ip, IP_V4_SIZE) == 0) {
		return this->l4_packet::proccess_packet(open_ports, ip, mask, dst);
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
	std::string packet_string = "";

	// case 2.1 - Packet entering the network - with the mask
	if(!is_src_in_net && is_dst_in_net){
		dst = RQ;
/*		this->as_string(packet_string);
		RQ.push_back(packet_string);*/
	}

	// case 2.2 - Packet exiting the network - with the mask
	else if(is_src_in_net && !is_dst_in_net){
		memcpy(this->src_ip, ip, IP_V4_SIZE);
		dst = TQ;
/*		this->as_string(packet_string);
		TQ.push_back(packet_string);*/
	}

	// case 2.3 - Packet is going threw - with the mask
	else if(!is_src_in_net && !is_dst_in_net){
		dst = TQ;
/*		this->as_string(packet_string);
		TQ.push_back(packet_string);*/
	}

	return true;
}

/**
 * @fn as_string
 * @brief Convert the packet to string.
 *
 * @param [out] packet - Packet as a string, ready to be stored in RQ/TQ.
 *
 * @return true in success, false in failure (e.g memory allocation failed).
 */
bool l3_packet::as_string(std::string &packet){

	for(int i = 0; i < IP_V4_SIZE; i++){
		if(i != IP_V4_SIZE -1)
			packet += std::to_string(this->src_ip[i]) + ".";
		else
			packet += std::to_string(this->src_ip[i]);
	}

	packet += "|";

	for(int i = 0; i < IP_V4_SIZE; i++){
		if(i != IP_V4_SIZE -1)
			packet += std::to_string(this->dst_ip[i]) + ".";
		else
			packet += std::to_string(this->dst_ip[i]);
	}

	std::string l4_str;
	l4_packet::as_string(l4_str);
	packet += "|"+ std::to_string(this->TTL) + "|" +
				  std::to_string(this->CS_l3) + "|" + l4_str;

	return true;
}

/**
 * @fn ip_with_mask
 * @brief return the ip with the mask number of left bits
 *
 * @param mask - number of lest bits to be considered
 * @param ip - the number in the shape of an array, that we
 * return his left bits only
 *
 * @return the "mask" number of left bits of the number "ip"
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
