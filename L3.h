/*
 * l3_packet.hpp
 *
 *  Created on: 21 Jun 2025
 *      Author: mamat
 */

#ifndef L3_H_
#define L3_H_

#include "L4.h"

enum L4_DELIMITERS_POS {
	l4_start = 4,	/* after src_ip|dst_ip|TTL|CS_l3| */
	l4_end = -1 	/* until end of string */
};

class l3_packet : public l4_packet {
	private:
	    uint8_t			src_ip[IP_V4_SIZE] = {0};
	    uint8_t 		dst_ip[IP_V4_SIZE] = {0};
	    unsigned int 	TTL = 0;
	    unsigned int 	CS_l3 = 0;

	    /**
	     * @fn ip_sum_diff
	     * @brief Compute the difference in byte-wise sum between two IPv4
	     * addresses.
	     *
	     * @param ip_a - First IPv4 address.
	     * @param ip_b - Second IPv4 address.
	     *
	     * @return Sum of bytes in `ip_b` minus sum of bytes in `ip_a`
	     */
	    int ip_sum_diff(uint8_t curr_ip[IP_V4_SIZE],
	    				uint8_t new_ip[IP_V4_SIZE]);

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
	    uint32_t ip_with_mask(uint8_t mask, uint8_t ip[IP_V4_SIZE]);

	public:
		/* C'tor and D'tor */
	    /**
	     * @fn l3_packet
	     * @brief Constructs an l3_packet from a given string
	     *
	     * @param [in] raw_data - string containing data formated as follows:
	     * 		src_ip(4 Bytes)|dst_ip(4 Bytes)|TTL(4 Bytes)|CS_ls(4 Bytes)
	     * 		|"l4_packet"
	     *
	     * @return new L3 packet
	     */
		l3_packet(const l3_packet& other) = default;
		/* Explisit Constructor */
		explicit l3_packet(const std::string& raw_data);
		/* distructor */
		virtual ~l3_packet() override = default;

		/* Getters */
		unsigned int get_CS_l3() const;

		/**
		 * @fn calc_sum
		 * @brief Calculates the byte-wise sum of all fields in the l3_packet.
		 *
		 * This includes the `TTL` field, each byte of `src_ip` and `dst_ip`
		 * and the result of the base class (`l4_packet`) `calc_sum()` function.
		 *
		 * @return The total sum as an unsigned integer.
		 */
		virtual unsigned int calc_sum() const;

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
		virtual bool validate_packet(open_port_vec open_ports,
                uint8_t ip[IP_V4_SIZE],
                uint8_t mask,
                uint8_t mac[MAC_SIZE]) override;

		/**
		 * @fn proccess_packet
		 * @brief Modify the packet and return the memory location it should be
		 *        stored in. In the case of local DRAM, the function will store
		 *        the packet as a string in the relevant 'open_port' struct.
		 *
		 * @param [in] open_ports - Vector containidata[i]ng all the NIC's open
		 * ports.
		 * @param [in] ip - NIC's IP address.
		 * @param [in] mask - NIC's mask; together with the IP, determines the
		 * NIC's
		 *        local net.
		 * @param [out] dst - Reference to enum that indicate the memory space
		 * where
		 *        the packet should be stored:
		 *         LOCAL_DRAM - the function stored it to the currect struct.
		 *         RQ - Should be stored as a string in RQ.
		 *         TQ - Should be stored as string in TQ.
		 *
		 * @return true in success,
		 * 			false in failure - dumping packet due to TTL or CS_l3 error.
		 *
		 */
		virtual bool proccess_packet(open_port_vec &open_ports,
	                                uint8_t ip[IP_V4_SIZE],
	                                uint8_t mask,
	                                memory_dest &dst) override;

		/**
		 * @fn as_string
		 * @brief Convert the packet to string.
		 *
		 * @param [out] packet - Packet as a string, ready to be stored in
		 * RQ/TQ.
		 *
		 * @return true always.
		 */
		virtual bool as_string(std::string &packet) override;
};

#endif /* L3_H_ */
