/*
 * l4.cpp
 *
 *  Created on: 19 Jun 2025
 *      Author: mamat
 */

#include "L4.h"
#include <cstring>
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

	/* Setters */
	void l4_packet::set_src_prt(unsigned short prt) { src_prt = prt; }
	void l4_packet::set_dst_prt(unsigned short prt) { dst_prt = prt; }
	void l4_packet::set_addrs(unsigned int addr) { addrs = addr; }
	void l4_packet::set_data(const uint8_t* d) { std::memcpy(data, d, PACKET_DATA_SIZE); }
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
		if(this->addrs < 0 || this->addrs >= 32){
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
     * @return true in success, false in failure (e.g memory allocation failed).
     */

    bool l4_packet::proccess_packet(open_port_vec &open_ports,
                            		uint8_t ip[IP_V4_SIZE],
									uint8_t mask,
									memory_dest &dst) {
    	if(!this->validate_packet(open_ports, ip, mask, NULL)){
    		return false;
    	}
    	for(open_port& port : open_ports){
    		if(this->dst_prt == port.dst_prt) {
    			memcpy(port.data[this->addrs], this->data, PACKET_DATA_SIZE);
    		}
    	}
    	dst = LOCAL_DRAM; // Local DRAM
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
    bool l4_packet::as_string(std::string &packet){
    	packet = std::to_string(this->src_prt) + "|" +
    			 std::to_string(this->dst_prt) + "|";
    	for(int i = 0; i < PACKET_DATA_SIZE; i++){
    		if(i != PACKET_DATA_SIZE -1)
    			packet += std::to_string(this->data[i]) + " ";
    		else
    			packet += std::to_string(this->data[i]);
    	}
    	return true;
    }
