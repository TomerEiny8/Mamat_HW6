/*
 * l3_packet.hpp
 *
 *  Created on: 21 Jun 2025
 *      Author: mamat
 */

#ifndef L4_H_
#define L3_H_

#include "packets.hpp"
#include "L4.h"

class l3_packet : public l4_packet {
	private:
	    unsigned int 	src_ip = 0;
	    unsigned int 	dst_ip = 0;
	    unsigned int 	TTL = 0;
	    unsigned int 	CS_l3 = 0;

	public:
		/* C'tor and D'tor */
		l3_packet(const l3_packet& other) = default;
		explicit l3_packet(const std::string& raw_data);
		virtual ~l3_packet() override = default;

		/* Getters */
		unsigned short get_src_ip() const;
		unsigned short get_dst_ip() const;
		unsigned int get_TTL() const;
		unsigned int get_CS_l3() const;

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

#endif /* L4_H_ */
