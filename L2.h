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

	    void write_mac(std::string &packet, uint8_t (&mac)[MAC_SIZE]);

	public:
		/* C'tor and D'tor */
		l2_packet(const l2_packet& other) = default;
		explicit l2_packet(const std::string& raw_data);
		virtual ~l2_packet() override = default;

		virtual unsigned int calc_sum() const;

		virtual bool validate_packet(open_port_vec open_ports,
                uint8_t ip[IP_V4_SIZE],
                uint8_t mask,
                uint8_t mac[MAC_SIZE]) override;

		virtual bool proccess_packet(open_port_vec &open_ports,
	                                uint8_t ip[IP_V4_SIZE],
	                                uint8_t mask,
	                                memory_dest &dst) override;

		virtual bool as_string(std::string &packet) override;
};

#endif /* L2_H_ */
