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

	    void extract_ip(const std::string ip_str,  uint8_t (&ip)[IP_V4_SIZE]);
	    uint32_t ip_with_mask(uint8_t mask, uint8_t ip[IP_V4_SIZE]);

	public:
		/* C'tor and D'tor */
		l3_packet(const l3_packet& other) = default;
		explicit l3_packet(const std::string& raw_data);
		virtual ~l3_packet() override = default;

		/* Getters */
		const uint8_t* get_src_ip() const;
		const uint8_t* get_dst_ip() const;
		unsigned int get_TTL() const;
		unsigned int get_CS_l3() const;
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

#endif /* L3_H_ */
