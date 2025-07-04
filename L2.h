/*
 * L2.h
 *
 *  Created on: 23 Jun 2025
 *      Author: mamat
 */

#ifndef L2_H_
#define L2_H_

#include "L3.h"

enum L3_DELIMITERS_POS {
	l3_start = 2,	/* after "src_mac | dst_mac |" */
	l3_end = 9	/* before CS_l3 */
};

class l2_packet : public l3_packet {
	private:
		uint8_t 		src_mac[MAC_SIZE] = {0};
		uint8_t 		dst_mac[MAC_SIZE] = {0};
	    unsigned int 	CS_l2 = 0;

	    /**
	     * @fn write_mac
	     * @brief 	Parses the MAC into the string in HEX
	     * 			with ":" as delimiter and "|" at the end
	     *
	     * @param [out] packet - Packet as a string.
	     * @param [in]  mac - MAC to parse
	     *
	     * @return None.
	     */
	    void write_mac(std::string &packet, uint8_t (&mac)[MAC_SIZE]);

	public:
		/* C'tor and D'tor */

	    /**
	     * @fn l2_packet
	     * @brief Constructs an l2_packet from a given string
	     *
	     * @param [in] raw_data - string containing data formated as follows:
	     * 		src_mac(6 Bytes)|dst_mac(6 Bytes)|"l3_packet"|CS_l2(4 Bytes)
	     *
	     * @return new L2 packet
	     */
		l2_packet(const l2_packet& other) = default;
		/* Explisit Constructor */
		explicit l2_packet(const std::string& raw_data);
		/* distructor */
		virtual ~l2_packet() override = default;

		/**
		 * @fn calc_sum
		 * @brief Calculates the byte-wise sum of all fields in the l2_packet.
		 *
		 * This includes each byte of `src_mac` and `dst_mac`
		 * and the result of the base class (`l3_packet`) `calc_sum()` function.
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
		virtual bool proccess_packet(open_port_vec &open_ports,
	                                uint8_t ip[IP_V4_SIZE],
	                                uint8_t mask,
	                                memory_dest &dst) override;

		/**
		 * @fn as_string
		 * @brief Converts the packet to string.
		 *
		 * @param [out] packet - Packet as a string.
		 *
		 * @return true always.
		 */
		virtual bool as_string(std::string &packet) override;
};

#endif /* L2_H_ */
