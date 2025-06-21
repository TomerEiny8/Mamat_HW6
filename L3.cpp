/*
 * l3.cpp
 *
 *  Created on: 21 Jun 2025
 *      Author: mamat
 */

#include "L3.h"

/**
 * @fn l4_packet
 * @brief Constructor from given string
 *
 * @param [in] raw_data - string containing data formated as follows:
 * 		src_port(2 Bytes)|dst_port(2 Bytes)|addrs(4 Byes)|data(32 Bytes)
 *
 * @return new L4 packet
 *
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
*/

	/* Getters */
	unsigned int l3_packet::get_src_ip() const { return this->src_ip; }
	unsigned int l3_packet::get_dst_ip() const { return this->dst_ip; }
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
		if(!l4_packet::validate_packet(open_ports, ip, mask, mac)){
			return false;
		}
		if(this->TTL <= 0){
			return false;
		}

		uint8_t sum = 0;
		std::string packet_string= generic_packet::extract_between_delimiters
												(l3::as_string(this),'|', 0);
		for(char c : packet_string){
			sum += static_cast<uint8_t>(c);
		}
		return true;
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

    bool l3_packet::proccess_packet(open_port_vec &open_ports,
                            		uint8_t ip[IP_V4_SIZE],
									uint8_t mask,
									memory_dest &dst) {

    	bool is_src_ip = memcmp(this->src_ip, ip, IP_V4_SIZE);
    	bool is_dst_ip = memcmp(this->dst_ip, ip, IP_V4_SIZE);

    	if(!this->validate_packet(open_ports, ip, mask, NULL){
    		return false;
    	}
    	// case 2.1 - Packet entering the network
    	if(!is_src_ip && is_dst_ip){
    		this->TTL--;
    		this->CS_l3--;
    		memory_dest = RQ;
    		RQ.push_back(l3_packet::as_string(this));
    		return true;
    	}
    	// case 2.2 - Packet exiting the network
    	if(is_src_ip && !is_dst_ip){
        	this->TTL--;
        	this->CS_l3--;
        	memcpy(this->src_ip, ip, IP_V4_SIZE);
        	memory_dest = TQ;
        	TQ.push_back(l3_packet::as_string(this));
        	return true;
        }

    	// case 2.3 - Packet is going threw
    	if(!is_src_ip && !is_dst_ip){
           	this->TTL--;
   	       	this->CS_l3--;
           	memory_dest = TQ;
           	TQ.push_back(l3_packet::as_string(this));
           	return true;
        }

    	// case 2.4 - NIC = ip_dst
    	l4_packet::proccess_packet(opent_ports, ip, mask, dst);
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
    	packet = std::to_string(this->src_ip) + "|" +
    			 std::to_string(this->dst_ip) + "|" +
				 std::to_string(this->TTL) + "|" +
				 std::to_string(this->CS_l3) + "|" +
    			 std::to_string(this->src_prt) + "|" +
    			 std::to_string(this->dst_prt) + "|" +
				 std::to_string(this->data);
    	return true;
    }


